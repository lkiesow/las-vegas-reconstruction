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
 * ViewerManager.cpp
 *
 *  Created on: 07.10.2010
 *      Author: Thomas Wiemann
 */

#include "ViewerManager.h"
#include "PerspectiveViewer.h"


ViewerManager::ViewerManager(QWidget* parent, const QGLWidget* shared)
{
	m_currentViewer = new PerspectiveViewer(parent, shared);
	m_parentWidget = parent;
}

ViewerManager::~ViewerManager()
{

}

Viewer* ViewerManager::current()
{
	return m_currentViewer;
}

void ViewerManager::addDataCollector(DataCollector* c)
{
	// Stub, currently support only one single viewer instance
	m_currentViewer->addDataObject(c);
}

void ViewerManager::removeDataCollector(DataCollector* c)
{
    // Stub, currently support only one single viewer instance
    m_currentViewer->removeDataObject(c);
}

void ViewerManager::updateDataObject(DataCollector* obj)
{
	m_currentViewer->updateDataObject(obj);
}
