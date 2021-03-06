/*
===============================================================================

  FILE:  lasreader.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@gmail.com
  
  COPYRIGHT:
  
    (c) 2007-2011, Martin Isenburg, LASSO - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "lasreader.hpp"

#include "lasindex.hpp"
#include "lasfilter.hpp"
#include "lastransform.hpp"

#include "lasreader_las.hpp"
#include "lasreader_bin.hpp"
#include "lasreader_shp.hpp"
#include "lasreader_qfit.hpp"
#include "lasreader_txt.hpp"
#include "lasreadermerged.hpp"

#include <stdlib.h>
#include <string.h>

LASreader::LASreader()
{
  npoints = 0;
  p_count = 0;
  read_simple = &LASreader::read_point_default;
  read_complex = 0;
  index = 0;
  filter = 0;
  transform = 0;
  r_min_x = 0;
  r_min_y = 0;
  r_max_x = 0;
  r_max_y = 0;
  t_ll_x = 0;
  t_ll_y = 0;
  t_size = 0;
  t_ur_x = 0;
  t_ur_y = 0;
  c_center_x = 0;
  c_center_y = 0;
  c_radius = 0;
  c_radius_squared = 0;
}
  
LASreader::~LASreader()
{
  if (index) delete index;
}

void LASreader::set_index(LASindex* index)
{
  if (this->index) delete this->index;
  this->index = index;
}

LASindex* LASreader::get_index() const
{
  return index;
}

void LASreader::set_filter(LASfilter* filter)
{
  this->filter = filter;
  if (filter && transform)
  {
    read_simple = &LASreader::read_point_filtered_and_transformed;
  }
  else if (filter)
  {
    read_simple = &LASreader::read_point_filtered;
  }
  else if (transform)
  {
    read_simple = &LASreader::read_point_transformed;
  }
  read_complex = &LASreader::read_point_default;
}

void LASreader::set_transform(LAStransform* transform)
{
  this->transform = transform;
  if (filter && transform)
  {
    read_simple = &LASreader::read_point_filtered_and_transformed;
  }
  else if (filter)
  {
    read_simple = &LASreader::read_point_filtered;
  }
  else if (transform)
  {
    read_simple = &LASreader::read_point_transformed;
  }
  read_complex = &LASreader::read_point_default;
}

void LASreader::reset_filter()
{
  if (filter) filter->reset();
}

BOOL LASreader::inside_tile(const F32 ll_x, const F32 ll_y, const F32 size)
{
  t_ll_x = ll_x;
  t_ll_y = ll_y;
  t_size = size;
  t_ur_x = ll_x + size;
  t_ur_y = ll_y + size;
  header.min_x = ll_x;
  header.min_y = ll_y;
  header.max_x = ll_x + size - 0.001f * header.x_scale_factor;
  header.max_y = ll_y + size - 0.001f * header.y_scale_factor;
  if (index) index->intersect_tile(ll_x, ll_y, size);
  if (filter || transform)
  {
    if (index)
      read_complex = &LASreader::read_point_inside_tile_indexed;
    else
      read_complex = &LASreader::read_point_inside_tile;
  }
  else
  {
    if (index)
      read_simple = &LASreader::read_point_inside_tile_indexed;
    else
      read_simple = &LASreader::read_point_inside_tile;
  }
  return TRUE;
}

BOOL LASreader::inside_circle(const F64 center_x, const F64 center_y, const F64 radius)
{
  c_center_x = center_x;
  c_center_y = center_y;
  c_radius = radius;
  c_radius_squared = radius*radius;
  header.min_x = center_x - radius;
  header.min_y = center_y - radius;
  header.max_x = center_x + radius;
  header.max_y = center_y + radius;
  if (index) index->intersect_circle(center_x, center_y, radius);
  if (filter || transform)
  {
    if (index)
      read_complex = &LASreader::read_point_inside_circle_indexed;
    else
      read_complex = &LASreader::read_point_inside_circle;
  }
  else
  {
    if (index)
      read_simple = &LASreader::read_point_inside_circle_indexed;
    else
      read_simple = &LASreader::read_point_inside_circle;
  }
  return TRUE;
}

BOOL LASreader::inside_rectangle(const F64 min_x, const F64 min_y, const F64 max_x, const F64 max_y)
{
  r_min_x = min_x;
  r_min_y = min_y;
  r_max_x = max_x;
  r_max_y = max_y;
  header.min_x = min_x;
  header.min_y = min_y;
  header.max_x = max_x;
  header.max_y = max_y;
  if (index) index->intersect_rectangle(min_x, min_y, max_x, max_y);
  if (filter || transform)
  {
    if (index)
      read_complex = &LASreader::read_point_inside_rectangle_indexed;
    else
      read_complex = &LASreader::read_point_inside_rectangle;
  }
  else
  {
    if (index)
      read_simple = &LASreader::read_point_inside_rectangle_indexed;
    else
      read_simple = &LASreader::read_point_inside_rectangle;
  }
  return TRUE;
}

BOOL LASreader::read_point_inside_tile()
{
  while (read_point_default())
  {
    if (point.inside_tile(t_ll_x, t_ll_y, t_ur_x, t_ur_y)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_inside_tile_indexed()
{
  while (index->seek_next((LASreader*)this))
  {
    if (read_point_default() && point.inside_tile(t_ll_x, t_ll_y, t_ur_x, t_ur_y)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_inside_circle()
{
  while (read_point_default())
  {
    if (point.inside_circle(c_center_x, c_center_y, c_radius_squared)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_inside_circle_indexed()
{
  while (index->seek_next((LASreader*)this))
  {
    if (read_point_default() && point.inside_circle(c_center_x, c_center_y, c_radius_squared)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_inside_rectangle()
{
  while (read_point_default())
  {
    if (point.inside_rectangle(r_min_x, r_min_y, r_max_x, r_max_y)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_inside_rectangle_indexed()
{
  while (index->seek_next((LASreader*)this))
  {
    if (read_point_default() && point.inside_rectangle(r_min_x, r_min_y, r_max_x, r_max_y)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_filtered()
{
  while ((this->*read_complex)())
  {
    if (!filter->filter(&point)) return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_transformed()
{
  if ((this->*read_complex)())
  {
    transform->transform(&point);
    return TRUE;
  }
  return FALSE;
}

BOOL LASreader::read_point_filtered_and_transformed()
{
  if (read_point_filtered())
  {
    transform->transform(&point);
    return TRUE;
  }
  return FALSE;
}

BOOL LASreadOpener::piped() const
{
  return (!file_names && use_stdin);
}

BOOL LASreadOpener::has_populated_header() const
{
  return (populate_header || (file_name && (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))));
}

void LASreadOpener::reset()
{
  file_name_current = 0;
  file_name = 0;
}

LASreader* LASreadOpener::open()
{
  if (file_names)
  {
    use_stdin = FALSE;
    if (file_name_current == file_name_number) return 0;
    if ((file_name_number > 1) && merged)
    {
      LASreaderMerged* lasreadermerged = new LASreaderMerged();
      if (use_alternate) lasreadermerged->use_alternate_reader();
      lasreadermerged->set_scale_factor(scale_factor);
      lasreadermerged->set_offset(offset);
      lasreadermerged->set_parse_string(parse_string);
      lasreadermerged->set_skip_lines(skip_lines);
      lasreadermerged->set_populate_header(populate_header);
      lasreadermerged->set_translate_intensity(translate_intensity);
      lasreadermerged->set_scale_intensity(scale_intensity);
      lasreadermerged->set_translate_scan_angle(translate_scan_angle);
      lasreadermerged->set_scale_scan_angle(scale_scan_angle);
      for (file_name_current = 0; file_name_current < file_name_number; file_name_current++) lasreadermerged->add_file_name(file_names[file_name_current]);
      if (!lasreadermerged->open())
      {
        fprintf(stderr,"ERROR: cannot open lasreadermerged with %d file names\n", file_name_number);
        delete lasreadermerged;
        return 0;
      }
      if (filter) lasreadermerged->set_filter(filter);
      if (transform) lasreadermerged->set_transform(transform);
      if (inside_tile) lasreadermerged->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
      if (inside_circle) lasreadermerged->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
      if (inside_rectangle) lasreadermerged->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
      return lasreadermerged;
    }
    else
    {
      file_name = file_names[file_name_current];
      file_name_current++;
      if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
      {
        LASreaderLAS* lasreaderlas;
        if (scale_factor == 0 && offset == 0)
          lasreaderlas = new LASreaderLAS();
        else if (scale_factor != 0 && offset == 0)
          lasreaderlas = new LASreaderLASrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          lasreaderlas = new LASreaderLASreoffset(offset[0], offset[1], offset[2]);
        else
          lasreaderlas = new LASreaderLASrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
        if (!lasreaderlas->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open lasreaderlas with file name '%s'\n", file_name);
          delete lasreaderlas;
          return 0;
        }
        LASindex* index = new LASindex();
        if (index->read(file_name))
          lasreaderlas->set_index(index);
        else
          delete index;
        if (filter) lasreaderlas->set_filter(filter);
        if (transform) lasreaderlas->set_transform(transform);
        if (inside_tile) lasreaderlas->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderlas->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderlas->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return lasreaderlas;
      }
      else if (strstr(file_name, ".bin") || strstr(file_name, ".BIN"))
      {
        LASreaderBIN* lasreaderbin;
        if (scale_factor == 0 && offset == 0)
          lasreaderbin = new LASreaderBIN();
        else if (scale_factor != 0 && offset == 0)
          lasreaderbin = new LASreaderBINrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          lasreaderbin = new LASreaderBINreoffset(offset[0], offset[1], offset[2]);
        else
          lasreaderbin = new LASreaderBINrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
        if (!lasreaderbin->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open lasreaderbin with file name '%s'\n", file_name);
          delete lasreaderbin;
          return 0;
        }
        LASindex* index = new LASindex();
        if (index->read(file_name))
          lasreaderbin->set_index(index);
        else
          delete index;
        if (filter) lasreaderbin->set_filter(filter);
        if (transform) lasreaderbin->set_transform(transform);
        if (inside_tile) lasreaderbin->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderbin->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderbin->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return lasreaderbin;
      }
      else if (strstr(file_name, ".shp") || strstr(file_name, ".SHP"))
      {
        LASreaderSHP* lasreadershp;
        if (scale_factor == 0 && offset == 0)
          lasreadershp = new LASreaderSHP();
        else if (scale_factor != 0 && offset == 0)
          lasreadershp = new LASreaderSHPrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          lasreadershp = new LASreaderSHPreoffset(offset[0], offset[1], offset[2]);
        else
          lasreadershp = new LASreaderSHPrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
        if (!lasreadershp->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open lasreadershp with file name '%s'\n", file_name);
          delete lasreadershp;
          return 0;
        }
        if (filter) lasreadershp->set_filter(filter);
        if (transform) lasreadershp->set_transform(transform);
        if (inside_tile) lasreadershp->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreadershp->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreadershp->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return lasreadershp;
      }
      else if (strstr(file_name, ".qi") || strstr(file_name, ".QI"))
      {
        LASreaderQFIT* lasreaderqfit;
        if (scale_factor == 0 && offset == 0)
          lasreaderqfit = new LASreaderQFIT();
        else if (scale_factor != 0 && offset == 0)
          lasreaderqfit = new LASreaderQFITrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          lasreaderqfit = new LASreaderQFITreoffset(offset[0], offset[1], offset[2]);
        else
          lasreaderqfit = new LASreaderQFITrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
        if (!lasreaderqfit->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open lasreaderqfit with file name '%s'\n", file_name);
          delete lasreaderqfit;
          return 0;
        }
        LASindex* index = new LASindex();
        if (index->read(file_name))
          lasreaderqfit->set_index(index);
        else
          delete index;
        if (filter) lasreaderqfit->set_filter(filter);
        if (transform) lasreaderqfit->set_transform(transform);
        if (inside_tile) lasreaderqfit->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderqfit->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderqfit->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return lasreaderqfit;
      }
      else
      {
        LASreaderTXT* lasreadertxt = new LASreaderTXT();
        lasreadertxt->set_translate_intensity(translate_intensity);
        lasreadertxt->set_scale_intensity(scale_intensity);
        lasreadertxt->set_translate_scan_angle(translate_scan_angle);
        lasreadertxt->set_scale_scan_angle(scale_scan_angle);
        lasreadertxt->set_scale_factor(scale_factor);
        lasreadertxt->set_offset(offset);
        if (number_extra_attributes)
        {
          for (I32 i = 0; i < number_extra_attributes; i++)
          {
            lasreadertxt->add_extra_attribute(extra_attribute_data_types[i], extra_attribute_names[i], extra_attribute_descriptions[i], extra_attribute_scales[i], extra_attribute_offsets[i]);
          }
        }
        if (!lasreadertxt->open(file_name, parse_string, skip_lines, populate_header))
        {
          fprintf(stderr,"ERROR: cannot open lasreadertxt with file name '%s'\n", file_name);
          delete lasreadertxt;
          return 0;
        }
        if (filter) lasreadertxt->set_filter(filter);
        if (transform) lasreadertxt->set_transform(transform);
        if (inside_tile) lasreadertxt->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreadertxt->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreadertxt->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return lasreadertxt;
      }
    }
  }
  else if (use_stdin)
  {
    use_stdin = FALSE; populate_header = TRUE;
    LASreaderLAS* lasreaderlas;
    if (scale_factor == 0 && offset == 0)
      lasreaderlas = new LASreaderLAS();
    else if (scale_factor != 0 && offset == 0)
      lasreaderlas = new LASreaderLASrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
    else if (scale_factor == 0 && offset != 0)
      lasreaderlas = new LASreaderLASreoffset(offset[0], offset[1], offset[2]);
    else
      lasreaderlas = new LASreaderLASrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
    if (!lasreaderlas->open(stdin))
    {
      fprintf(stderr,"ERROR: cannot open lasreaderlas from stdin \n");
      delete lasreaderlas;
      return 0;
    }
    if (filter) lasreaderlas->set_filter(filter);
    if (transform) lasreaderlas->set_transform(transform);
    if (inside_tile) lasreaderlas->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
    if (inside_circle) lasreaderlas->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
    if (inside_rectangle) lasreaderlas->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
    return lasreaderlas;
  }
  else
  {
    return 0;
  }
}

BOOL LASreadOpener::reopen(LASreader* lasreader)
{
  if (file_names)
  {
    if ((file_name_number > 1) && merged)
    {
      LASreaderMerged* lasreadermerged = (LASreaderMerged*)lasreader;
      if (!lasreadermerged->reopen())
      {
        fprintf(stderr,"ERROR: cannot reopen lasreadermerged\n");
        return FALSE;
      }
      if (filter) lasreadermerged->reset_filter();
      if (inside_tile) lasreadermerged->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
      if (inside_circle) lasreadermerged->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
      if (inside_rectangle) lasreadermerged->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
      return TRUE;
    }
    else
    {
      if (!file_name) return FALSE;
      if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
      {
        LASreaderLAS* lasreaderlas = (LASreaderLAS*)lasreader;
        if (!lasreaderlas->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen lasreaderlas with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) lasreaderlas->reset_filter();
        if (inside_tile) lasreaderlas->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderlas->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderlas->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else if (strstr(file_name, ".bin") || strstr(file_name, ".BIN"))
      {
        LASreaderBIN* lasreaderbin = (LASreaderBIN*)lasreader;
        if (!lasreaderbin->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen lasreaderbin with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) lasreaderbin->reset_filter();
        if (inside_tile) lasreaderbin->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderbin->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderbin->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else if (strstr(file_name, ".shp") || strstr(file_name, ".SHP"))
      {
        LASreaderSHP* lasreadershp = (LASreaderSHP*)lasreader;
        if (!lasreadershp->reopen(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen lasreadershp with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) lasreadershp->reset_filter();
        if (inside_tile) lasreadershp->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreadershp->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreadershp->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else if (strstr(file_name, ".qi") || strstr(file_name, ".QI"))
      {
        LASreaderQFIT* lasreaderqfit = (LASreaderQFIT*)lasreader;
        if (!lasreaderqfit->reopen(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen lasreaderqfit with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) lasreaderqfit->reset_filter();
        if (inside_tile) lasreaderqfit->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreaderqfit->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreaderqfit->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else
      {
        LASreaderTXT* lasreadertxt = (LASreaderTXT*)lasreader;
        if (!lasreadertxt->reopen(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen lasreadertxt with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) lasreadertxt->reset_filter();
        if (inside_tile) lasreadertxt->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) lasreadertxt->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) lasreadertxt->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
    }
  }
  else
  {
    fprintf(stderr,"ERROR: no lasreader input specified\n");
    return FALSE;
  }
}

LASwaveform13reader* LASreadOpener::open_waveform13(const LASheader* lasheader)
{
  if (lasheader->point_data_format < 4) return 0;
  if (lasheader->vlr_wave_packet_descr == 0) return 0;
  if (get_file_name() == 0) return 0;
  LASwaveform13reader* waveform13reader = new LASwaveform13reader();
  if ((lasheader->global_encoding & 2) && (lasheader->start_of_waveform_data_packet_record > lasheader->offset_to_point_data))
  {
    if (waveform13reader->open(get_file_name(), lasheader->start_of_waveform_data_packet_record, lasheader->vlr_wave_packet_descr))
    {
      return waveform13reader;
    }
  }
  else
  {
    if (waveform13reader->open(get_file_name(), 0, lasheader->vlr_wave_packet_descr))
    {
      return waveform13reader;
    }
  }
  delete waveform13reader;
  return 0;
}

void LASreadOpener::usage() const
{
  fprintf(stderr,"Supported LAS Inputs\n");
  fprintf(stderr,"  -i lidar.las\n");
  fprintf(stderr,"  -i lidar.laz\n");
  fprintf(stderr,"  -i lidar1.las lidar2.las lidar3.las -merged\n");
  fprintf(stderr,"  -i *.las\n");
  fprintf(stderr,"  -i flight0??.laz flight1??.laz -single\n");
  fprintf(stderr,"  -i terrasolid.bin\n");
  fprintf(stderr,"  -i esri.shp\n");
  fprintf(stderr,"  -i nasa.qi\n");
  fprintf(stderr,"  -i lidar.txt -iparse xyzti -iskip 2 (on-the-fly from ASCII)\n");
  fprintf(stderr,"  -i lidar.txt -iparse xyzi -itranslate_intensity 1024\n");
  fprintf(stderr,"  -lof file_list.txt\n");
  fprintf(stderr,"  -stdin (pipe from stdin)\n");
  fprintf(stderr,"  -rescale 0.1 0.1 0.1\n");
  fprintf(stderr,"  -reoffset 600000 4000000 0\n");
}

BOOL LASreadOpener::parse(int argc, char* argv[])
{
  int i;
  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      LASfilter().usage();
      LAStransform().usage();
      usage();
      return TRUE;
    }
    else if (strcmp(argv[i],"-i") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: file_name or wild_card\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
#ifdef _WIN32
        add_file_name_windows(argv[i]);
#else
        add_file_name(argv[i]);
#endif
        *argv[i]='\0';
        i+=1;
      } while (i < argc && *argv[i] != '-');
      i-=1;
    }
    else if (strcmp(argv[i],"-inside_tile") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: ll_x, ll_y, size\n", argv[i]);
        return FALSE;
      }
      if (inside_tile == 0) inside_tile = new F32[3];
      inside_tile[0] = (F32)atof(argv[i+1]);
      inside_tile[1] = (F32)atof(argv[i+2]);
      inside_tile[2] = (F32)atof(argv[i+3]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-inside_circle") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: center_x, center_y, radius\n", argv[i]);
        return FALSE;
      }
      if (inside_circle == 0) inside_circle = new F64[3];
      inside_circle[0] = atof(argv[i+1]);
      inside_circle[1] = atof(argv[i+2]);
      inside_circle[2] = atof(argv[i+3]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-inside") == 0 || strcmp(argv[i],"-inside_rectangle") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x, min_y, max_x, max_y\n", argv[i]);
        return FALSE;
      }
      if (inside_rectangle == 0) inside_rectangle = new F64[4];
      inside_rectangle[0] = atof(argv[i+1]);
      inside_rectangle[1] = atof(argv[i+2]);
      inside_rectangle[2] = atof(argv[i+3]);
      inside_rectangle[3] = atof(argv[i+4]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-stdin") == 0)
    {
      use_stdin = TRUE;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-lof") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: list_of_files\n", argv[i]);
        return FALSE;
      }
      FILE* file = fopen(argv[i+1], "r");
      if (file == 0)
      {
        fprintf(stderr, "ERROR: cannot open '%s'\n", argv[i+1]);
        return FALSE;
      }
      char line[1024];
      while (fgets(line, 1024, file))
      {
        // find end of line
        int len = strlen(line) - 1;
        // remove extra white spaces and line return at the end 
        while (len > 0 && ((line[len] == '\n') || (line[len] == ' ') || (line[len] == '\t') || (line[len] == '\012')))
        {
          line[len] = '\0';
          len--;
        }
        add_file_name(line);
      }
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-rescale") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: rescale_x, rescale_y, rescale_z\n", argv[i]);
        return FALSE;
      }
      F64 scale_factor[3];
      scale_factor[0] = atof(argv[i+1]);
      scale_factor[1] = atof(argv[i+2]);
      scale_factor[2] = atof(argv[i+3]);
      set_scale_factor(scale_factor);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-reoffset") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: reoffset_x, reoffset_y, reoffset_z\n", argv[i]);
        return FALSE;
      }
      F64 offset[3];
			offset[0] = atof(argv[i+1]);
			offset[1] = atof(argv[i+2]);
			offset[2] = atof(argv[i+3]);
      set_offset(offset);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-files_are_flightlines") == 0)
    {
      use_alternate = TRUE;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-itranslate_intensity") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset\n", argv[i]);
        return FALSE;
      }
      set_translate_intensity((F32)atof(argv[i+1]));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-iscale_intensity") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      set_scale_intensity((F32)atof(argv[i+1]));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-itranslate_scan_angle") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset\n", argv[i]);
        return FALSE;
      }
      set_translate_scan_angle((F32)atof(argv[i+1]));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-iscale_scan_angle") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      set_scale_scan_angle((F32)atof(argv[i+1]));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-iadd_extra") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: data_type name description\n", argv[i]);
        return FALSE;
      }
      if (((i+4) < argc) && (argv[i+4][0] != '-'))
      {
        if (((i+5) < argc) && (argv[i+5][0] != '-'))
        {
          add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3], atof(argv[i+4]), atof(argv[i+5]));
          *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; *argv[i+5]='\0'; i+=5;
        }
        else
        {
          add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3], atof(argv[i+4]));
          *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4;
        }
      }
      else
      {
        add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3]);
        *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
      }
    }
    else if (strcmp(argv[i],"-iparse") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: string\n", argv[i]);
        return FALSE;
      }
      set_parse_string(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-iskip") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: number_of_lines\n", argv[i]);
        return FALSE;
      }
      set_skip_lines(atoi(argv[i+1]));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-single") == 0)
    {
      set_merged(FALSE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-merged") == 0)
    {
      set_merged(TRUE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-populate") == 0)
    {
      set_populate_header(TRUE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-do_not_populate") == 0)
    {
      set_populate_header(FALSE);
      *argv[i]='\0';
    }
  }

  if (filter) filter->clean();
  else filter = new LASfilter();
  if (!filter->parse(argc, argv))
  {
    delete filter;
    return FALSE;
  }
  if (!filter->active())
  {
    delete filter;
    filter = 0;
  }

  if (transform) transform->clean();
  else transform = new LAStransform();
  if (!transform->parse(argc, argv))
  {
    delete transform;
    return FALSE;
  }
  if (!transform->active())
  {
    delete transform;
    transform = 0;
  }

  return TRUE;
}

const char* LASreadOpener::get_file_name() const
{
  if (file_name)
    return file_name;
  if (file_name_number)
    return file_names[0];
  return 0;
}

const char* LASreadOpener::get_file_name(U32 number) const
{
  return file_names[number];
}

I32 LASreadOpener::get_file_format(U32 number) const
{
  if (strstr(file_names[number], ".las") || strstr(file_names[number], ".LAS"))
  {
    return LAS_TOOLS_FORMAT_LAS;
  }
  else if (strstr(file_names[number], ".laz") || strstr(file_names[number], ".LAZ"))
  {
    return LAS_TOOLS_FORMAT_LAZ;
  }
  else if (strstr(file_names[number], ".bin") || strstr(file_names[number], ".BIN"))
  {
    return LAS_TOOLS_FORMAT_BIN;
  }
  else if (strstr(file_names[number], ".shp") || strstr(file_names[number], ".SHP"))
  {
    return LAS_TOOLS_FORMAT_SHP;
  }
  else if (strstr(file_names[number], ".qi") || strstr(file_names[number], ".QI"))
  {
    return LAS_TOOLS_FORMAT_QFIT;
  }
  else
  {
    return LAS_TOOLS_FORMAT_TXT;
  }
}

void LASreadOpener::set_merged(const BOOL merged)
{
  this->merged = merged;
}

BOOL LASreadOpener::get_merged() const
{
  return merged;
}

void LASreadOpener::set_file_name(const char* file_name, BOOL unique)
{
#ifdef _WIN32
  add_file_name_windows(file_name, unique);
#else
  add_file_name(file_name, unique);
#endif
}

BOOL LASreadOpener::add_file_name(const char* file_name, BOOL unique)
{
  if (unique)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++)
    {
      if (strcmp(file_names[i], file_name) == 0)
      {
        return FALSE;
      }
    }
  }
  if (file_name_number == file_name_allocated)
  {
    if (file_names)
    {
      file_name_allocated *= 2;
      file_names = (char**)realloc(file_names, sizeof(char*)*file_name_allocated);
    }
    else
    {
      file_name_allocated = 16;
      file_names = (char**)malloc(sizeof(char*)*file_name_allocated);
    }
    if (file_names == 0)
    {
      fprintf(stderr, "ERROR: alloc for file_names pointer array failed at %d\n", file_name_allocated);
    }
  }
  file_names[file_name_number] = strdup(file_name);
  file_name_number++;
  return TRUE;
}

void LASreadOpener::delete_file_name(U32 file_name_id)
{
  if (file_name_id < file_name_number)
  {
    U32 i;
    free(file_names[file_name_id]);
    for (i = file_name_id+1; i < file_name_number; i++)
    {
      file_names[i-1] = file_names[i];
    }
  }
  file_name_number--;
}

BOOL LASreadOpener::set_file_name_current(U32 file_name_id)
{
  if (file_name_id < file_name_number)
  {
    file_name_current = file_name_id;
    return TRUE;
  }
  return FALSE;
}

#ifdef _WIN32
#include <windows.h>
void LASreadOpener::add_file_name_windows(const char* file_name, BOOL unique)
{
  HANDLE h;
  WIN32_FIND_DATA info;
  h = FindFirstFile(file_name, &info);
  if (h != INVALID_HANDLE_VALUE)
  {
    // find the path
    int len = strlen(file_name);
    while (len && file_name[len] != '\\') len--;
    if (len)
    {
      len++;
      char full_file_name[512];
      strncpy(full_file_name, file_name, len);
	    do
	    {
        sprintf(&full_file_name[len], "%s", info.cFileName);
        add_file_name(full_file_name, unique);
  	  } while (FindNextFile(h, &info));
    }
    else
    {
      do
      {
        add_file_name(info.cFileName, unique);
  	  } while (FindNextFile(h, &info));
    }
	  FindClose(h);
  }
}
#endif

U32 LASreadOpener::get_file_name_number() const
{
  return file_name_number;
}

void LASreadOpener::set_parse_string(const char* parse_string)
{
  if (this->parse_string) free(this->parse_string);
  if (parse_string)
  {
    this->parse_string = strdup(parse_string);
  }
  else
  {
    this->parse_string = 0;
  }
}

const char* LASreadOpener::get_parse_string() const
{
  return parse_string;
}

void LASreadOpener::set_scale_factor(const F64* scale_factor)
{
  if (scale_factor)
  {
    if (this->scale_factor == 0) this->scale_factor = new F64[3];
    this->scale_factor[0] = scale_factor[0];
    this->scale_factor[1] = scale_factor[1];
    this->scale_factor[2] = scale_factor[2];
  }
  else if (this->scale_factor)
  {
    delete [] this->scale_factor;
    this->scale_factor = 0;
  }
}

void LASreadOpener::set_offset(const F64* offset)
{
  if (offset)
  {
    if (this->offset == 0) this->offset = new F64[3];
    this->offset[0] = offset[0];
    this->offset[1] = offset[1];
    this->offset[2] = offset[2];
  }
  else if (this->offset)
  {
    delete [] this->offset;
    this->offset = 0;
  }
}

void LASreadOpener::set_translate_intensity(F32 translate_intensity)
{
  this->translate_intensity = translate_intensity;
}

void LASreadOpener::set_scale_intensity(F32 scale_intensity)
{
  this->scale_intensity = scale_intensity;
}

void LASreadOpener::set_translate_scan_angle(F32 translate_scan_angle)
{
  this->translate_scan_angle = translate_scan_angle;
}

void LASreadOpener::set_scale_scan_angle(F32 scale_scan_angle)
{
  this->scale_scan_angle = scale_scan_angle;
}

void LASreadOpener::add_extra_attribute(I32 data_type, const char* name, const char* description, F64 scale, F64 offset)
{
  extra_attribute_data_types[number_extra_attributes] = data_type;
  extra_attribute_names[number_extra_attributes] = (name ? strdup(name) : 0);
  extra_attribute_descriptions[number_extra_attributes] = (description ? strdup(description) : 0);
  extra_attribute_scales[number_extra_attributes] = scale;
  extra_attribute_offsets[number_extra_attributes] = offset;
  number_extra_attributes++;
}

void LASreadOpener::set_skip_lines(I32 skip_lines)
{
  this->skip_lines = skip_lines;
}

void LASreadOpener::set_populate_header(BOOL populate_header)
{
  this->populate_header = populate_header;
}

BOOL LASreadOpener::active() const
{
  return ((file_name_current < file_name_number) || use_stdin);
}

LASreadOpener::LASreadOpener()
{
  file_names = 0;
  file_name = 0;
  merged = TRUE;
  use_stdin = FALSE;
  scale_factor = 0;
  offset = 0;
  use_alternate = FALSE;
  translate_intensity = 0.0f;
  scale_intensity = 1.0f;
  translate_scan_angle = 0.0f;
  scale_scan_angle = 1.0f;
  number_extra_attributes = 0;
  for (I32 i = 0; i < 10; i++)
  {
    extra_attribute_data_types[i] = 0;
    extra_attribute_names[i] = 0;
    extra_attribute_descriptions[i] = 0;
    extra_attribute_scales[i] = 1.0;
    extra_attribute_offsets[i] = 0.0;
  }
  parse_string = 0;
  skip_lines = 0;
  populate_header = TRUE;
  file_name_number = 0;
  file_name_allocated = 0;
  file_name_current = 0;
  inside_tile = 0;
  inside_circle = 0;
  inside_rectangle = 0;
  filter = 0;
  transform = 0;
}

LASreadOpener::~LASreadOpener()
{
  if (file_names)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++) free(file_names[i]);
    free(file_names);
  }
  if (parse_string) free(parse_string);
  if (scale_factor) delete [] scale_factor;
  if (offset) delete [] offset;
  if (inside_tile) delete [] inside_tile;
  if (inside_circle) delete [] inside_circle;
  if (inside_rectangle) delete [] inside_rectangle;
  if (filter) delete filter;
  if (transform) delete transform;
}
