/*******************************************************************************
 * Copyright © 2011 Universität Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA  02111-1307, USA
 ******************************************************************************/


/**
 * @file       mesh2pointcloud.cpp
 * @brief      
 * @details    
 * @author     Lars Kiesow (lkiesow), lkiesow@uos.de
 * @version    20108
 * @date       Created:       2012-01-08 02:49:26
 * @date       Last modified: 2012-01-08 02:49:30
 */

#include <iostream>
#include <io/ModelFactory.hpp>
#include <io/Timestamp.hpp>
#include <map>
#include <string>
#include <cstring>
#include <omp.h>


/* How often should the triangles be divided? */
size_t iterations    = 1;
/* The minimum size (bounding box area) for a triangle to be divided */
size_t minimum_size  = 10;
/* If we want only a point cloud or also a mesh as output. */
bool   save_mesh     = false;
std::string ply_mode = "PLY_ASCII";



void printHelp( char * name ) {

    std::cout << "Usage: " << name << " [options] infile outfile" << std::endl
            << "Options:" << std::endl
            << "   -h   Show this help and exit." << std::endl
            << "   -i   How often should the triangles be divided?." << std::endl
            << "   -s   Set the minimum size (bounding box area) for a triangle to be divided." << std::endl
            << "   -w   Write also a mesh as output." << std::endl
            << "   -m   Set mode of PLY output files. If output file" << std::endl
            << "        format is not PLY this option will have no effect." << std::endl
            << "   -j   Number of jobs to be scheduled parallel." << std::endl
            << "        Positive integer or “auto” (default)" << std::endl;

}


void parseArgs( int argc, char ** argv ) {

    /* Parse options */
    char c;
    while ( ( c = getopt( argc, argv, "hwi:j:s:m:" ) ) != -1 ) {
        switch (c) {
            case 'h':
                printHelp( *argv );
                exit( EXIT_SUCCESS );
            case 'i':
                iterations = atoi( optarg ); break;
            case 's':
                minimum_size = atoi( optarg ); break;
            case 'm':
                ply_mode = std::string( optarg ); break;
            case 'j':
                if ( !strcmp( optarg, "auto" ) ) {
                    omp_set_num_threads( omp_get_num_procs() );
                } else {
                    omp_set_num_threads( 
                            atoi( optarg ) > 1 
                            ? atoi( optarg )
                            : omp_get_num_procs() );
                }
                break;
            case 'w':
                save_mesh = true;
        }
    }

    /* Check, if we got enough command line arguments */
    if ( argc - optind != 2 ) {
        printHelp( *argv );
        exit( EXIT_SUCCESS );
    }
    
}


lssr::coord<float> getMidPoint( lssr::coord<float> a, lssr::coord<float> b ) {

    lssr::coord<float> c;
    c.x = (a.x + b.x) / 2;
    c.y = (a.y + b.y) / 2;
    c.z = (a.z + b.z) / 2;
    return c;

}


int main( int argc, char ** argv )
{

    parseArgs( argc, argv );


    /* Load mesh. */
    std::cout << lssr::timestamp << "Loading mesh from »" << argv[ optind ]
        << "«…" << std::endl;
    lssr::ModelPtr model( lssr::ModelFactory::readModel( argv[ optind ] ) );
    lssr::MeshBufferPtr mesh( model->m_mesh );

    /* Get loaded data (vertices and faces). */
    size_t nvertices, nfaces;
    lssr::coord3fArr   old_vertices( mesh->getIndexedVertexArray( nvertices ) );
    lssr::uintArr      old_faces( mesh->getFaceArray( nfaces ) );

    /* Calculate size for new buffer. */
    size_t new_nvertices( nvertices ), new_nfaces( nfaces );
    for ( size_t i(0); i < iterations; i++ ) {
        /* There will be three new vertices for each existing face per iteration. */
        new_nvertices += new_nfaces * 3;
        /* There will be four new faces for each existing face per iteration.
         * But we do not need the old faces anymore. */
        new_nfaces *= 4;
    }


    /* Create new buffer for new faces and points. */
    lssr::coord3fArr   vertices( new lssr::coord<float>[ new_nvertices ] );
    lssr::uintArr      faces( new unsigned int[ 3 * 4 * nfaces ] );

    /* We want to keep the old vertices. Thus we copy them to the new vertex
     * buffer. */
    for ( size_t i(0); i < nvertices; i++ ) {
        vertices[i] = old_vertices[i];
    }


    for ( size_t it(0); it < iterations; it++ ) {

        size_t j( nvertices ), k( 0 );
        /* Run through faces and divide them. */
        for ( size_t i(0); i < nfaces; i++ ) {

            /* Get old vertices/points for this face. */
            lssr::coord<float> a( old_vertices[ old_faces[ i * 3     ] ] );
            lssr::coord<float> b( old_vertices[ old_faces[ i * 3 + 1 ] ] );
            lssr::coord<float> c( old_vertices[ old_faces[ i * 3 + 2 ] ] );

            /* Set new vertices/points. */
            vertices[ j     ] = getMidPoint( a, b );
            vertices[ j + 1 ] = getMidPoint( b, c );
            vertices[ j + 2 ] = getMidPoint( c, a );

            /* Set new faces. */
            faces[ k      ] = old_faces[ i * 3     ]; /* a  */
            faces[ k +  1 ] = j;                      /* ab */
            faces[ k +  2 ] = j + 2;                  /* ca */
            faces[ k +  3 ] = old_faces[ i * 3 + 1 ]; /* b  */
            faces[ k +  4 ] = j;                      /* ab */
            faces[ k +  5 ] = j + 1;                  /* bc */
            faces[ k +  6 ] = old_faces[ i * 3 + 2 ]; /* c  */
            faces[ k +  7 ] = j + 1;                  /* bc */
            faces[ k +  8 ] = j + 2;                  /* ca */
            faces[ k +  9 ] = j;                      /* ab */
            faces[ k + 10 ] = j + 1;                  /* bc */
            faces[ k + 11 ] = j + 2;                  /* ca */

            j +=  3;
            k += 12;
        }

        /* Finally replace »old« faces with new ones (for next iteration), set
         * the proper amount of vertices/faces we got and create a new face
         * buffer. */
        old_faces = faces;
        nvertices = j;
        nfaces    = k / 3; /* Remember that faces is an interlaced array. */
        faces.reset( new unsigned int[ 3 * 4 * nfaces ] );

    }

	lssr::ModelPtr m( new lssr::Model );
    m->m_pointCloud.reset( new lssr::PointBuffer );
    m->m_pointCloud->setIndexedPointArray( vertices, nvertices );

    /* Set mesh buffer if we also want to save the mesh. */
    if ( save_mesh ) {
        m->m_mesh.reset( new lssr::MeshBuffer );
        m->m_mesh->setIndexedVertexArray( vertices, nvertices );
        m->m_mesh->setFaceArray( old_faces, nfaces );
    }


    /* Save in new format. */
    std::cout << lssr::timestamp << "Writing point cloud to »"
        << argv[ optind + 1 ] << "«…" << std::endl;
    std::multimap< std::string, std::string > save_opts;
    /* Build call string */
    {
        std::string s("");
        for ( size_t i(0); i < argc-1; i++ )
        {
            s += std::string( argv[i] ) + " ";
        }
        s += argv[ argc-1 ];
        save_opts.insert( std::pair< std::string, std::string >( "comment", s ) );
    }
    save_opts.insert( std::pair< std::string, std::string >( "comment",
                "Created with las-vegas-reconstruction (colorize): "
                "http://las-vegas.uos.de/" ) );
    save_opts.insert( std::pair<std::string, std::string>( "ply_mode", ply_mode ));
    lssr::ModelFactory::saveModel( m, argv[ optind + 1 ], save_opts );

    std::cout << lssr::timestamp << "Done." << std::endl;
    return EXIT_SUCCESS;

}
