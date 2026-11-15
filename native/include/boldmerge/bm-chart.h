#pragma once

#include <boldmerge/bm-theme-scope.h>

G_BEGIN_DECLS

#define BM_TYPE_CHART_SERIES (bm_chart_series_get_type())
G_DECLARE_FINAL_TYPE(BmChartSeries, bm_chart_series, BM, CHART_SERIES, GObject)

BmChartSeries *bm_chart_series_new(const char *label);
void bm_chart_series_append(BmChartSeries *self,
                            double value,
                            const char *formatted_value);
const char *bm_chart_series_get_label(BmChartSeries *self);
guint bm_chart_series_get_length(BmChartSeries *self);
double bm_chart_series_get_value(BmChartSeries *self, guint index);
const char *bm_chart_series_get_formatted_value(BmChartSeries *self,
                                                 guint index);

#define BM_TYPE_CHART_TOOLTIP (bm_chart_tooltip_get_type())
G_DECLARE_FINAL_TYPE(BmChartTooltip, bm_chart_tooltip, BM, CHART_TOOLTIP,
                     GtkPopover)

GtkWidget *bm_chart_tooltip_new(void);
void bm_chart_tooltip_set_heading(BmChartTooltip *self, const char *heading);
void bm_chart_tooltip_clear(BmChartTooltip *self);
void bm_chart_tooltip_append(BmChartTooltip *self,
                             const char *label,
                             const char *value);

#define BM_TYPE_CHART (bm_chart_get_type())
G_DECLARE_FINAL_TYPE(BmChart, bm_chart, BM, CHART, GtkBox)

GtkWidget *bm_chart_new(const char *label, const char *summary);
void bm_chart_append_category(BmChart *self, const char *category);
void bm_chart_append_series(BmChart *self, BmChartSeries *series);
void bm_chart_clear(BmChart *self);
guint bm_chart_get_category_count(BmChart *self);
guint bm_chart_get_series_count(BmChart *self);
void bm_chart_set_active_index(BmChart *self, int index);
int bm_chart_get_active_index(BmChart *self);
void bm_chart_set_summary(BmChart *self, const char *summary);
void bm_chart_set_table(BmChart *self, GtkWidget *table);
GtkWidget *bm_chart_get_table(BmChart *self);
BmChartTooltip *bm_chart_get_tooltip(BmChart *self);
void bm_chart_set_theme_scope(BmChart *self, BmThemeScope *scope);

G_END_DECLS
