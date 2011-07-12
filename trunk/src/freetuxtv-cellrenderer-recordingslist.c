/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * freetuxtv
 * Copyright (C) Eric Beuque 2011 <eric.beuque@gmail.com>
	 * 
 * freetuxtv is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 * 
 * freetuxtv is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "freetuxtv-cellrenderer-recordingslist.h"
#include "freetuxtv-i18n.h"

G_DEFINE_TYPE (FreetuxTVCellRendererRecordingsList, freetuxtv_cellrenderer_recordingslist, GTK_TYPE_CELL_RENDERER);

enum
{
	PROP_TITLE = 1,
	PROP_LOGOPATH,
	PROP_BEGINTIME,
	PROP_ENDTIME,
	PROP_STATUS,
};


static void
freetuxtv_cellrenderer_recordingslist_get_property (GObject *object,
    guint param_id, GValue *value, GParamSpec *psec)
{
	FreetuxTVCellRendererRecordingsList *self = FREETUXTV_CELLRENDERER_RECORDINGSLIST(object);

	switch (param_id){
		case PROP_TITLE:
			g_value_set_string(value, self->szTitle);
			break;
		case PROP_LOGOPATH:
			g_value_set_string(value, self->szLogoPath);
			break;
		case PROP_BEGINTIME:
			g_value_set_int64(value, self->beginTime);
			break;
		case PROP_ENDTIME:
			g_value_set_int64(value, self->endTime);
			break;
		case PROP_STATUS:
			g_value_set_int (value, self->status);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, psec);
			break;
	}
}

static void
freetuxtv_cellrenderer_recordingslist_set_property (GObject *object,
    guint param_id, const GValue *value, GParamSpec *pspec)
{	
	FreetuxTVCellRendererRecordingsList *self = FREETUXTV_CELLRENDERER_RECORDINGSLIST(object);

	switch (param_id){
		case PROP_TITLE:
			if(self->szTitle){
				g_free(self->szTitle);
				self->szTitle = NULL;
			}
			self->szTitle = g_strdup(g_value_get_string(value));
			break;
		case PROP_LOGOPATH:
			if(self->szLogoPath){
				g_free(self->szLogoPath);
				self->szLogoPath = NULL;
			}
			self->szLogoPath = g_strdup(g_value_get_string(value));
			break;
		case PROP_BEGINTIME:
			self->beginTime = g_value_get_int64 (value);
			break;
		case PROP_ENDTIME:
			self->endTime = g_value_get_int64 (value);
			break;
		case PROP_STATUS:
			self->status = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
			break;
	}
}

#if GTK_API_VERSION == 3
static void
freetuxtv_cellrenderer_recordingslist_get_size (
    GtkCellRenderer *cell,
    GtkWidget *widget, const GdkRectangle *cell_area,
    gint *x_offset, gint *y_offset,
    gint *width, gint *height)
#else
static void
freetuxtv_cellrenderer_recordingslist_get_size (
    GtkCellRenderer *cell,
    GtkWidget *widget, GdkRectangle *cell_area,
    gint *x_offset, gint *y_offset,
    gint *width, gint *height)
#endif
{

	// FreetuxTVCellRendererRecordingsList *self = FREETUXTV_CELLRENDERER_RECORDINGSLIST (cell);

	gint calc_width;
	gint calc_height;

	gint cell_xpad;
	gint cell_ypad;
	gfloat cell_xalign;
	gfloat cell_yalign;
	GtkAllocation allocation;

#if GTK_API_VERSION == 3
	gtk_cell_renderer_get_padding(cell, &cell_xpad, &cell_ypad);
	gtk_cell_renderer_get_alignment (cell, &cell_xalign, &cell_yalign);
	gtk_widget_get_allocation(widget, &allocation);
#else
	cell_xpad = cell->xpad;
	cell_ypad = cell->ypad;
	cell_xalign = cell->xalign;
	cell_yalign = cell->yalign;
	allocation = widget->allocation;
#endif

	calc_width  = (gint) cell_xpad * 2;
	calc_height = (gint) cell_ypad * 2;
	calc_height+=40;

	if (cell_area){
		calc_width += allocation.width;
	}

	if (width)
		*width = calc_width;

	if (height)
		*height = calc_height;

	if (cell_area){
		if (x_offset){
			*x_offset = cell_xalign * (cell_area->width - calc_width);
			*x_offset = MAX (*x_offset, 0);
		}

		if (y_offset){
			*y_offset = cell_yalign * (cell_area->height - calc_height);
			*y_offset = MAX (*y_offset, 0);
		}
	}
}

#if GTK_API_VERSION == 3
static void
freetuxtv_cellrenderer_recordingslist_render (GtkCellRenderer *cell,
    cairo_t *cr,
    GtkWidget *widget,
    const GdkRectangle *background_area,
    const GdkRectangle *cell_area,
    GtkCellRendererState flags)
#else
static void
freetuxtv_cellrenderer_recordingslist_render (GtkCellRenderer *cell,
    GdkWindow       *window,
    GtkWidget       *widget,
    GdkRectangle    *background_area,
    GdkRectangle    *cell_area,
    GdkRectangle    *expose_area,
    guint            flags)
#endif

{
	FreetuxTVCellRendererRecordingsList *self = FREETUXTV_CELLRENDERER_RECORDINGSLIST (cell);
	GtkStateType state;
	gint width, height;
	gint x_offset, y_offset;
	PangoLayout *layout;
	gchar* szTmp;
	gint64 duration;

	gboolean bHasFocus;

#if GTK_API_VERSION == 3
	GtkStyleContext *pStyleContext;
	pStyleContext = gtk_widget_get_style_context (GTK_WIDGET(cell));
	GdkWindow* window = gtk_widget_get_window  (GTK_WIDGET(cell));
	bHasFocus = gtk_widget_has_focus (GTK_WIDGET(cell));
#else
	GtkStyle* pStyle;
	pStyle = widget->style;
	bHasFocus = GTK_WIDGET_HAS_FOCUS (cell);
#endif
	
	if (bHasFocus) {			
		state = GTK_STATE_ACTIVE;
	} else {
		state = GTK_STATE_NORMAL;
	}

	freetuxtv_cellrenderer_recordingslist_get_size (cell, widget, cell_area,
	    &x_offset, &y_offset,
	    &width, &height);

	if ((flags & GTK_CELL_RENDERER_SELECTED) != 0){
		state = GTK_STATE_SELECTED;
	}

	int cell_xpad;
	int cell_ypad;
	
	// Draw the title
	layout = gtk_widget_create_pango_layout (widget,
	    self->szTitle);
	pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
	
#if GTK_API_VERSION == 3
	gtk_cell_renderer_get_padding (cell, &cell_xpad, &cell_ypad);

	gtk_render_layout (pStyleContext, cr,
	    cell_xpad * 2, cell_ypad,
	    layout);
#else
	cell_xpad = cell->xpad;
	cell_ypad = cell->ypad;
	
	gtk_paint_layout (widget->style, window, state,
	    TRUE, NULL, widget, NULL,
	    cell_xpad * 2, cell_ypad,
	    layout);
#endif

	// Draw the channel logo
	GdkPixbuf* pLogo;
	GdkPixbuf* pDestPixbuf;
	pLogo = gdk_pixbuf_new_from_file(self->szLogoPath, NULL);

	pDestPixbuf = gdk_pixbuf_scale_simple(pLogo, 20, 20, GDK_INTERP_HYPER);

#if GTK_API_VERSION == 3
	gdk_cairo_set_source_pixbuf(cr, pDestPixbuf, (double)cell_xpad, (double)cell_ypad);
	cairo_paint(cr);
#else
	gdk_draw_pixbuf (GDK_DRAWABLE(window), NULL, pDestPixbuf,
	    0, 0, cell->xpad + 1, cell_area->y + cell->ypad + 17 + 1,
	    -1, -1,
	    GDK_RGB_DITHER_NONE, 0,0);
#endif

	if(pDestPixbuf){
		g_object_unref(pDestPixbuf);
		pDestPixbuf = NULL;
	}
	if(pLogo){
		g_object_unref(pLogo);
		pLogo = NULL;
	}

	// Draw time
	duration = (self->endTime - self->beginTime) / (G_USEC_PER_SEC * 60);
	szTmp = g_strdup_printf(_("(S%d) - %lld min"), self->status, duration);
	layout = gtk_widget_create_pango_layout (widget, szTmp);
	pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);

#if GTK_API_VERSION == 3
	gtk_render_layout (pStyleContext, cr,
	    cell_xpad * 2 + 25, cell_area->y+20,
	    layout);
#else
	gtk_paint_layout (widget->style, window, state,
	    TRUE, NULL, widget, NULL,
	    cell_xpad * 2 + 25, cell_area->y+20,
	    layout);
#endif
	
	if(szTmp){
		g_free(szTmp);
		szTmp = NULL;
	}
}

static void
freetuxtv_cellrenderer_recordingslist_init (FreetuxTVCellRendererRecordingsList *object)
{
#if GTK_API_VERSION == 3
	gtk_cell_renderer_set_padding (GTK_CELL_RENDERER(object), 2, 2);
#else
	GTK_CELL_RENDERER(self)->mode = GTK_CELL_RENDERER_MODE_INERT;
	GTK_CELL_RENDERER(self)->xpad = 2;
	GTK_CELL_RENDERER(self)->ypad = 2;
#endif

	object->szTitle = NULL;
	object->szLogoPath = NULL;
	object->beginTime = 0;
	object->endTime = 0;
}

static void
freetuxtv_cellrenderer_recordingslist_finalize (GObject *object)
{
	FreetuxTVCellRendererRecordingsList *self;
	self = FREETUXTV_CELLRENDERER_RECORDINGSLIST(object);

	G_OBJECT_CLASS (freetuxtv_cellrenderer_recordingslist_parent_class)->finalize (object);

	if(self->szTitle){
		g_free(self->szTitle);
		self->szTitle = NULL;
	}
	if(self->szLogoPath){
		g_free(self->szLogoPath);
		self->szLogoPath = NULL;
	}
}

static void
freetuxtv_cellrenderer_recordingslist_class_init (FreetuxTVCellRendererRecordingsListClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtkCellRendererClass* parent_class = GTK_CELL_RENDERER_CLASS (klass);

	object_class->finalize = freetuxtv_cellrenderer_recordingslist_finalize;

	object_class->get_property = freetuxtv_cellrenderer_recordingslist_get_property;
	object_class->set_property = freetuxtv_cellrenderer_recordingslist_set_property;

	parent_class->get_size = freetuxtv_cellrenderer_recordingslist_get_size;
	parent_class->render   = freetuxtv_cellrenderer_recordingslist_render;

	g_object_class_install_property (object_class,
	    PROP_TITLE,
	    g_param_spec_string ("title",
		    "Title",
		    "The title of the recording to display in the cell",
		    NULL,
		    G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
	    PROP_LOGOPATH,
	    g_param_spec_string ("logopath",
		    "LogoPath",
		    "Path of the logo to display in the cell",
		    NULL,
		    G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
	    PROP_BEGINTIME,
	    g_param_spec_int64 ("begintime",
		    "BeginTime",
		    "Recording begin time",
		    G_MININT64,
		    G_MAXINT64,
		    0,
		    G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
	    PROP_ENDTIME,
	    g_param_spec_int64 ("endtime",
		    "EndTime",
		    "Recording end time",
		    G_MININT64,
		    G_MAXINT64,
		    0,
		    G_PARAM_READWRITE));

	g_object_class_install_property (object_class,
	    PROP_STATUS,
	    g_param_spec_int ("status",
		    "Status",
		    "Recording status",
		    G_MININT,
		    G_MAXINT,
		    0,
		    G_PARAM_READWRITE));
}

GtkCellRenderer *
freetuxtv_cellrenderer_recordingslist_new (void)
{
	return g_object_new(FREETUXTV_TYPE_CELLRENDERER_RECORDINGSLIST, NULL);
}