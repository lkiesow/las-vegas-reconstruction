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


 /*
 * Options.h
 *
 *  Created on: Nov 21, 2010
 *      Author: Thomas Wiemann
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

using std::ostream;
using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace reconstruct{

using namespace boost::program_options;

/**
 * @brief A class to parse the program options for the reconstruction
 * 		  executable.
 */
class Options {
public:

	/**
	 * @brief 	Ctor. Parses the command parameters given to the main
	 * 		  	function of the program
	 */
	Options(int argc, char** argv);
	virtual ~Options();

	/**
	 * @brief	Returns the given voxelsize
	 */
	float 	getVoxelsize()const;

	/**
	 * @brief	Returns the number of used threads
	 */
	int 	getNumThreads() const;

	/**
	 * @brief	Prints a usage message to stdout.
	 */
	bool	printUsage() const;

	/**
	 * @brief	Returns true if an output filen name was set
	 */
	bool	filenameSet() const;

	/**
	 * @brief 	Returns true if the face normals of the
	 * 			reconstructed mesh should be saved to an
	 * 			extra file ("face_normals.nor")
	 */
	bool	saveFaceNormals() const;

	/**
	 *@brief    Returns true of region coloring is enabled.
	 */
	bool    colorRegions() const;

	/**
	 * @brief	Returns true if the interpolated normals
	 * 			should be saved in the putput file
	 */
	bool    saveNormals() const;

    /**
     * @brief   Returns true if the Marching Cubes grid should be save
     */
    bool    saveGrid() const;

    /**
     * @brief   Returns true if the original points should be stored
     *          together with the reconstruction
     */
    bool    saveOriginalData() const;


	/**
	 * @brief 	Returns true if cluster optimization is enabled
	 */
	bool 	optimizePlanes() const;

	/**
	 * @brief 	Indicates whether to save the used points
	 * 			together with the interpolated normals.
	 */
	bool 	savePointNormals() const;

	/**
	 * @brief	If true, normals should be calculated even if
	 * 			they are already given in the input file
	 */
	bool	recalcNormals() const;

    /**
     * @brief   If true, RANSAC based normal estimation is used
     */
    bool    useRansac() const;

	/**
	 * @brief	If true, textures will be generated during
     *          finalization of mesh.
	 */
	bool	generateTextures() const;

	/**
	 * @brief	Returns the number of neighbors
	 * 			for normal interpolation
	 */
	int     getKi() const;

	/**
	 * @brief	Returns the number of neighbors used for
	 * 			initial normal estimation
	 */
	int     getKn() const;

	/**
	 * @brief	Returns the number of neighbors used for distance
	 * 			function evaluation
	 */
	int     getKd() const;

	/**
	  * @brief Return whether the mesh should be retesselated or not.
	  */
	bool	  retesselate() const;

	/**
	 * @brief	Returns the output file name
	 */
	string 	getInputFileName() const;

	/**
	 * @brief   Returns the number of intersections. If the return value
	 *          is positive it will be used for reconstruction instead of
	 *          absolute voxelsize.
	 */
	int     getIntersections() const;


	/**
	 * @brief   Returns to number plane optimization iterations
	 */
	int getPlaneIterations() const;

	/**
	 * @brief   Returns the name of the used point cloud handler.
	 */
	string getPCM() const;

    /**
     * @brief   Returns the name of the used point cloud handler.
     */
    string getDecomposition() const;


	/**
	 * @brief   Returns the normal threshold for plane optimization.
	 */
	float getNormalThreshold() const;

	/**
	 * @brief   Returns the threshold for the size of small
	 *          region deletion after plane optimization.
	 */
	int   getSmallRegionThreshold() const;

	/**
	 * @brief   Minimum value for plane optimzation
	 */
	int   getMinPlaneSize() const;

	/**
	 * @brief   Returns the number of dangling artifacts to remove from
	 *          a created mesh.
	 */
	int   getDanglingArtifacts() const;

	/**
	 * @brief   Returns the region threshold for hole filling
	 */
	int   getFillHoles() const;

	/**
	 * @brief 	Returns the maximum recursion depth for region growing
	 */
	int getDepth() const;

	/**
	 * @brief   Returns the texel size for texture resolution
	 */
	float getTexelSize() const;

	/**
	 * @brief   Returns the sharp feature threshold when using sharp feature decomposition
	 */
	float getSharpFeatureThreshold() const;

	/**
		 * @brief   Returns the sharp corner threshold when using sharp feature decomposition
		 */
	float getSharpCornerThreshold() const;

    /**
     * @brief   Returns the fusion threshold for tesselation
     */
    float getLineFusionThreshold() const;

    /**
     * @brief   Wheter to extend the grid. Enable by default.
     */
    bool extrude() const;

private:

	/// The set voxelsize
	float 				            m_voxelsize;

	/// The number of uesed threads
	int				                m_numThreads;

	/// The internally used variable map
	variables_map			        m_variables;

	/// The internally used option description
	options_description 		    m_descr;

	/// The internally used positional option desription
	positional_options_description 	m_pdescr;

	/// The putput file name for face normals
	string 				            m_faceNormalFile;

	/// The number of used default values
	int                             m_numberOfDefaults;

	/// The number of neighbors for distance function evaluation
	int                             m_kd;

	/// The number of neighbors for normal estimation
	int                             m_kn;

	/// The number of neighbors for normal interpolation
	int                             m_ki;

	/// The number of intersections used for reconstruction
	int                             m_intersections;

	/// Whether or not the mesh should be retesselated while being finalized
	bool						  m_retesselate;
	
    /// Whether or not the mesh should be retesselated while being finalized
	bool						   m_generateTextures;

	/// The used point cloud manager
	string                          m_pcm;

	/// Number of iterations for plane optimzation
	int                             m_planeIterations;

	/// Threshold for plane optimization
	float                           m_planeNormalThreshold;

	/// Threshold for small ragions
	int                             m_smallRegionThreshold;

	/// Number of dangling artifacts to remove
	int                             m_rda;

	/// Threshold for hole filling
	int                             m_fillHoles;

	/// Threshold for plane optimization
	int                             m_minPlaneSize;

	/// Maximum recursion depth for region growing
	int								m_depth;

	/// Texel size
	float                           m_texelSize;

	/// Threshold for line fusing when tesselating
	float                           m_lineFusionThreshold;

	/// Sharp feature threshold when using sharp feature decomposition
	float 							m_sft;

	/// Sharp corner threshold when using sharp feature decomposition
	float 							m_sct;
};


/// Overlaoeded outpur operator
inline ostream& operator<<(ostream& os, const Options &o)
{
	cout << "##### Program options: " << endl;
	if(o.getIntersections() > 0)
	{
	    cout << "##### Intersections \t\t: " << o.getIntersections() << endl;
	}
	else
	{
	    cout << "##### Voxelsize \t\t: " << o.getVoxelsize() << endl;
	}
	cout << "##### Input file \t\t: "         << o.getInputFileName()   << endl;
	cout << "##### Number of threads \t: "    << o.getNumThreads()      << endl;
	cout << "##### Point cloud manager \t: " << o.getPCM()             << endl;
	if(o.useRansac())
	{
	    cout << "##### Use RANSAC\t\t: YES" << endl;
	}
	else
	{
	    cout << "##### Use RANSAC\t\t: NO" << endl;
	}
	cout << "##### Voxel decomposition: \t: " << o.getDecomposition()   << endl;
	cout << "##### k_n \t\t\t: "              << o.getKn()              << endl;
	cout << "##### k_i \t\t\t: "              << o.getKi()              << endl;
	cout << "##### k_d \t\t\t: "              << o.getKd()              << endl;
	if(o.getDecomposition() == "SF")
	{
		cout << "##### Sharp feature threshold \t: " << o.getSharpFeatureThreshold() << endl;
		cout << "##### Sharp corner threshold \t: " << o.getSharpCornerThreshold() << endl;
	}
	if(o.retesselate())
	{
		cout << "##### Retesselate \t\t: YES"     << endl;
		cout << "##### Line fusion threshold \t: " << o.getLineFusionThreshold() << endl;
	}
	if(o.saveFaceNormals())
	{
		cout << "##### Write Face Normals \t: YES" << endl;
	}

	if(o.getFillHoles())
	{
	    cout << "##### Fill holes \t\t: " << o.getFillHoles() << endl;
	}
	else
	{
	    cout << "##### Fill holes \t\t: NO" << endl;
	}

	if(o.getDanglingArtifacts())
	{
	    cout << "##### Remove DAs \t\t: " << o.getDanglingArtifacts() << endl;
	}
	else
	{
	    cout << "##### Remove DAs \t\t: NO" << endl;
	}

	if(o.optimizePlanes())
	{
		cout << "##### Optimize Planes \t\t: YES" << endl;
		cout << "##### Plane iterations\t\t: " << o.getPlaneIterations() << endl;
		cout << "##### Normal threshold \t\t: " << o.getNormalThreshold() << endl;
		cout << "##### Region threshold\t\t: " << o.getSmallRegionThreshold() << endl;
	}
	if(o.saveNormals())
	{
		cout << "##### Save normals \t\t: YES" << endl;
	}
	if(o.saveOriginalData())
	{
	    cout << "##### Save input data \t\t: YES" << endl;
	}
	if(o.colorRegions())
	{
	    cout << "##### Color regions \t\t: YES" << endl;
	}
	if(o.recalcNormals())
	{
		cout << "##### Recalc normals \t\t: YES" << endl;
	}
	if(o.savePointNormals())
	{
	    cout << "##### Save points normals \t: YES" << endl;
	}
	if(o.generateTextures())
	{
	    cout << "##### Generate Textures \t: YES" << endl;
	    cout << "##### Texel size \t\t: " << o.getTexelSize() << endl;
	}
	if(o.getDepth())
	{
	    cout << "##### Recursion depth \t\t: " << o.getDepth() << endl;
	}
	return os;
}

} // namespace reconstruct


#endif /* OPTIONS_H_ */
