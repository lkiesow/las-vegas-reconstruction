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
 * Static3DDataCollector.cpp
 *
 *  Created on: 08.10.2010
 *      Author: Thomas Wiemann
 */

#include "Static3DDataCollector.h"

Static3DDataCollector::Static3DDataCollector(Renderable* renderable, string name, CustomTreeWidgetItem* item)
	: DataCollector(renderable, name, item) { m_renderable->setActive(true);}

ViewerType Static3DDataCollector::supportedViewerType()
{
	return PERSPECTIVE_VIEWER;
}
