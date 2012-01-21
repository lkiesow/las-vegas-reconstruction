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


lssr::coord<float> getMidPoint( lssr::coord<float> a, lssr::coord<float> b ) {

    lssr::coord<float> c;
    c.x = (a.x + b.x) / 2;
    c.y = (a.y + b.y) / 2;
    c.z = (a.z + b.z) / 2;
    return c;

}


int main( int argc, char ** argv )
{

    if ( argc != 3 )
    {
        std::cout << "Usage: " << *argv << "%s [options] infile outfile"
            << std::endl;
        return EXIT_SUCCESS;
    }

    /* Load mesh. */
    std::cout << lssr::timestamp << "Loading mesh from »" << argv[1]
        << "«…" << std::endl;
    lssr::ModelPtr model( lssr::ModelFactory::readModel( argv[1] ) );
    lssr::MeshBufferPtr mesh( model->m_mesh );

    size_t nvertices, nfaces;
    lssr::coord3fArr   old_vertices( mesh->getIndexedVertexArray( nvertices ) );
    lssr::uintArr      old_faces( mesh->getFaceArray( nfaces ) );
    lssr::coord3fArr   points( new lssr::coord<float>[ nvertices + ( 3 * nfaces ) ] );
    lssr::uintArr      faces( new unsigned int[ 3 * 4 * nfaces ] );

    /* First: Copy old vertices. */
    for ( size_t i(0); i < nvertices; i++ ) {
        points[i] = old_vertices[i];
    }

    size_t j( nvertices ), k( 0 );
    for ( size_t i(0); i < nfaces; i++ ) {
        /* Get old vertices/points for this face. */
        lssr::coord<float> a( old_vertices[ old_faces[ i * 3     ] ] );
        lssr::coord<float> b( old_vertices[ old_faces[ i * 3 + 1 ] ] );
        lssr::coord<float> c( old_vertices[ old_faces[ i * 3 + 2 ] ] );
        /* Set new vertices/points. */
        points[ j     ] = getMidPoint( a, b );
        points[ j + 1 ] = getMidPoint( b, c );
        points[ j + 2 ] = getMidPoint( c, a );
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

	lssr::ModelPtr m( new lssr::Model );
    m->m_pointCloud.reset( new lssr::PointBuffer );
    m->m_pointCloud->setIndexedPointArray( points, nvertices + ( 3 * nfaces ) );
    m->m_mesh.reset( new lssr::MeshBuffer );
    m->m_mesh->setIndexedVertexArray( points, nvertices + ( 3 * nfaces ) );
    m->m_mesh->setFaceArray( faces, 4 * nfaces );


    /* Save in new format. */
    std::cout << lssr::timestamp << "Writing point cloud to »" << argv[2]
        << "«…" << std::endl;
    std::multimap< std::string, std::string > save_opts;
    save_opts.insert( std::pair<std::string, std::string>( "ply_mode", "PLY_ASCII" ));
	 lssr::ModelFactory::saveModel( m, argv[2], save_opts );

    std::cout << lssr::timestamp << "Done." << std::endl;
    return EXIT_SUCCESS;

}
