/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */


/**
 * IOFactory.cpp
 *
 *  @date 24.08.2011
 *  @author Thomas Wiemann
 */

#include "AsciiIO.hpp"
#include "PLYIO.hpp"
#include "UosIO.hpp"
#include "ObjIO.hpp"
#include "LasIO.hpp"
#include "PCDIO.hpp"
#include "ModelFactory.hpp"

#include "Timestamp.hpp"
#include "Progress.hpp"

#include <boost/filesystem.hpp>

namespace lssr
{

ModelPtr ModelFactory::readModel( std::string filename )
{
    ModelPtr m;

    // Check extension
    boost::filesystem::path selectedFile( filename );
    std::string extension = selectedFile.extension().c_str();

    // Try to parse given file
    BaseIO* io = 0;
    if(extension == ".ply")
    {
        io = new PLYIO;
    }
    else if(extension == ".pts" || extension == ".3d" || extension == ".xyz")
    {
        io = new AsciiIO;
    }
    else if (extension == ".obj")
    {
        io = new ObjIO;
    }
    else if (extension == ".las")
    {
        io = new LasIO;
    }
#ifdef _USE_PCL_
    else if (extension == ".pcd")
    {
        io = new PCDIO;
    }
#endif /* _USE_PCL_ */
    else if (extension == "")
    {
        io = new UosIO;
    }

    // Return data model
    if( io )
    {
        m = io->read( filename );
        delete io;
    }

    return m;

}

void ModelFactory::saveModel( ModelPtr m, std::string filename,
        std::multimap< std::string, std::string > options )
{
    // Get file exptension
    boost::filesystem::path selectedFile(filename);
    std::string extension = selectedFile.extension().c_str();

    BaseIO* io = 0;

    // Create suitable io
    if(extension == ".ply")
    {
        io = new PLYIO;
    }
    else if (extension == ".pts" || extension == ".3d" || extension == ".xyz")
    {
        io = new AsciiIO;
    }
    else if ( extension == ".obj" )
    {
        io = new ObjIO;
    }
    else if (extension == ".pcd")
    {
        io = new PCDIO;
    }

    // Save model
    if(io)
    {
        io->save( filename, options, m );
        delete io;
    }
    else
    {
        cout << timestamp << "File format " << extension
            << " is currrently not supported." << endl;
    }



}

}
