#pragma once
#include <boldmerge/bm-theme-scope.h>
G_BEGIN_DECLS
typedef enum { BM_KPI_CHANGE_NEUTRAL, BM_KPI_CHANGE_FAVORABLE, BM_KPI_CHANGE_UNFAVORABLE } BmKpiChangeTone;
#define BM_TYPE_KPI_CHANGE_TONE (bm_kpi_change_tone_get_type())
GType bm_kpi_change_tone_get_type(void) G_GNUC_CONST;
#define BM_TYPE_KPI (bm_kpi_get_type())
G_DECLARE_FINAL_TYPE(BmKpi,bm_kpi,BM,KPI,GtkBox)
GtkWidget *bm_kpi_new(const char *label, const char *value);
void bm_kpi_set_label(BmKpi *self, const char *label);
void bm_kpi_set_value(BmKpi *self, const char *value);
void bm_kpi_set_change(BmKpi *self, const char *change,
                       BmKpiChangeTone tone);
void bm_kpi_set_comparison(BmKpi *self, const char *comparison);
void bm_kpi_set_sparkline(BmKpi *self, const double *values, gsize length);
void bm_kpi_set_theme_scope(BmKpi *self, BmThemeScope *scope);
G_END_DECLS
