#include "bm-private.h"

struct _BmCombobox {
  GtkBox parent_instance;
  GtkLabel *label_widget;
  GtkDropDown *control;
  GtkStringList *labels;
  GtkLabel *error_widget;
  GPtrArray *values;
  char *label;
  char *value;
  char *error;
  gboolean required;
  gboolean valid;
  BmThemeScope *scope;
  gulong notify_id;
  char *custom_class;
};
G_DEFINE_FINAL_TYPE(BmCombobox, bm_combobox, GTK_TYPE_BOX)
enum { PROP_0, PROP_LABEL, PROP_VALUE, PROP_REQUIRED, PROP_ERROR, PROP_VALID, PROP_THEME_SCOPE, N_PROPERTIES };
enum { SIGNAL_CHANGED, N_SIGNALS };
static GParamSpec *properties[N_PROPERTIES];
static guint signals[N_SIGNALS];

static void update_valid(BmCombobox *s) {
  gboolean valid = (s->error == NULL || *s->error == '\0') && (!s->required || (s->value != NULL && *s->value != '\0'));
  if (s->valid != valid) { s->valid = valid; g_object_notify_by_pspec(G_OBJECT(s), properties[PROP_VALID]); }
  if (valid) gtk_widget_remove_css_class(GTK_WIDGET(s), "bm-invalid"); else gtk_widget_add_css_class(GTK_WIDGET(s), "bm-invalid");
  gtk_accessible_update_state(GTK_ACCESSIBLE(s->control), GTK_ACCESSIBLE_STATE_INVALID, valid ? GTK_ACCESSIBLE_INVALID_FALSE : GTK_ACCESSIBLE_INVALID_TRUE, -1);
}
static void apply_theme(BmCombobox *s) { if (s->scope) bm_theme_scope_apply_context_to_widget(s->scope, GTK_WIDGET(s), &s->custom_class); }
static void on_theme(GObject *o, GParamSpec *p, gpointer d) { (void)o; (void)p; apply_theme(BM_COMBOBOX(d)); }
static void on_selected(GObject *o, GParamSpec *p, gpointer d) {
  (void)o; (void)p; BmCombobox *s = BM_COMBOBOX(d); guint i = gtk_drop_down_get_selected(s->control);
  const char *v = i < s->values->len ? g_ptr_array_index(s->values, i) : NULL;
  if (g_strcmp0(s->value, v) == 0) return;
  g_free(s->value); s->value = g_strdup(v); update_valid(s);
  g_object_notify_by_pspec(G_OBJECT(s), properties[PROP_VALUE]);
  const char *label = i < g_list_model_get_n_items(G_LIST_MODEL(s->labels)) ? gtk_string_list_get_string(s->labels, i) : NULL;
  g_signal_emit(s, signals[SIGNAL_CHANGED], 0, s->value, label);
}
void bm_combobox_append(BmCombobox *s, const char *text, const char *value) { g_return_if_fail(BM_IS_COMBOBOX(s)); gtk_string_list_append(s->labels, text); g_ptr_array_add(s->values, g_strdup(value ? value : text)); }
void bm_combobox_remove_all(BmCombobox *s) { g_return_if_fail(BM_IS_COMBOBOX(s)); gtk_string_list_splice(s->labels, 0, s->values->len, NULL); g_ptr_array_set_size(s->values, 0); gtk_drop_down_set_selected(s->control, GTK_INVALID_LIST_POSITION); update_valid(s); }
const char *bm_combobox_get_label(BmCombobox *s) { g_return_val_if_fail(BM_IS_COMBOBOX(s), NULL); return s->label; }
const char *bm_combobox_get_value(BmCombobox *s) { g_return_val_if_fail(BM_IS_COMBOBOX(s), NULL); return s->value; }
void bm_combobox_set_value(BmCombobox *s, const char *value) { g_return_if_fail(BM_IS_COMBOBOX(s)); for (guint i=0;i<s->values->len;i++) if (g_strcmp0(g_ptr_array_index(s->values,i),value)==0) { gtk_drop_down_set_selected(s->control,i); on_selected(G_OBJECT(s->control),NULL,s); return; } gtk_drop_down_set_selected(s->control,GTK_INVALID_LIST_POSITION); on_selected(G_OBJECT(s->control),NULL,s); }
gboolean bm_combobox_get_required(BmCombobox *s) { g_return_val_if_fail(BM_IS_COMBOBOX(s),FALSE); return s->required; }
void bm_combobox_set_required(BmCombobox *s,gboolean v) { g_return_if_fail(BM_IS_COMBOBOX(s)); v=!!v; if(s->required==v)return; s->required=v; update_valid(s); gtk_accessible_update_property(GTK_ACCESSIBLE(s->control),GTK_ACCESSIBLE_PROPERTY_REQUIRED,v,-1); g_object_notify_by_pspec(G_OBJECT(s),properties[PROP_REQUIRED]); }
const char *bm_combobox_get_error(BmCombobox *s) { g_return_val_if_fail(BM_IS_COMBOBOX(s),NULL); return s->error; }
void bm_combobox_set_error(BmCombobox *s,const char *e) { g_return_if_fail(BM_IS_COMBOBOX(s)); if(g_strcmp0(s->error,e)==0)return; g_free(s->error);s->error=g_strdup(e);gtk_label_set_text(s->error_widget,e?e:"");gtk_widget_set_visible(GTK_WIDGET(s->error_widget),e&&*e);update_valid(s);g_object_notify_by_pspec(G_OBJECT(s),properties[PROP_ERROR]); }
gboolean bm_combobox_get_valid(BmCombobox *s){g_return_val_if_fail(BM_IS_COMBOBOX(s),FALSE);return s->valid;}gboolean bm_combobox_validate(BmCombobox*s){g_return_val_if_fail(BM_IS_COMBOBOX(s),FALSE);update_valid(s);return s->valid;}GtkDropDown*bm_combobox_get_control(BmCombobox*s){g_return_val_if_fail(BM_IS_COMBOBOX(s),NULL);return s->control;}
void bm_combobox_set_theme_scope(BmCombobox*s,BmThemeScope*x){g_return_if_fail(BM_IS_COMBOBOX(s));if(s->scope==x)return;if(s->scope&&s->notify_id)g_signal_handler_disconnect(s->scope,s->notify_id);bm_theme_scope_clear_context_from_widget(GTK_WIDGET(s),&s->custom_class);g_set_object(&s->scope,x);s->notify_id=x?g_signal_connect(x,"notify",G_CALLBACK(on_theme),s):0;apply_theme(s);g_object_notify_by_pspec(G_OBJECT(s),properties[PROP_THEME_SCOPE]);}
static void get_prop(GObject*o,guint id,GValue*v,GParamSpec*p){BmCombobox*s=BM_COMBOBOX(o);switch(id){case PROP_LABEL:g_value_set_string(v,s->label);break;case PROP_VALUE:g_value_set_string(v,s->value);break;case PROP_REQUIRED:g_value_set_boolean(v,s->required);break;case PROP_ERROR:g_value_set_string(v,s->error);break;case PROP_VALID:g_value_set_boolean(v,s->valid);break;case PROP_THEME_SCOPE:g_value_set_object(v,s->scope);break;default:G_OBJECT_WARN_INVALID_PROPERTY_ID(o,id,p);}}
static void set_label(BmCombobox*s,const char*l){if(g_strcmp0(s->label,l)==0)return;g_free(s->label);s->label=g_strdup(l);gtk_label_set_text(s->label_widget,l?l:"");gtk_accessible_update_property(GTK_ACCESSIBLE(s->control),GTK_ACCESSIBLE_PROPERTY_LABEL,l,-1);g_object_notify_by_pspec(G_OBJECT(s),properties[PROP_LABEL]);}
static void set_prop(GObject*o,guint id,const GValue*v,GParamSpec*p){BmCombobox*s=BM_COMBOBOX(o);switch(id){case PROP_LABEL:set_label(s,g_value_get_string(v));break;case PROP_VALUE:bm_combobox_set_value(s,g_value_get_string(v));break;case PROP_REQUIRED:bm_combobox_set_required(s,g_value_get_boolean(v));break;case PROP_ERROR:bm_combobox_set_error(s,g_value_get_string(v));break;case PROP_THEME_SCOPE:bm_combobox_set_theme_scope(s,g_value_get_object(v));break;default:G_OBJECT_WARN_INVALID_PROPERTY_ID(o,id,p);}}
static void dispose(GObject*o){BmCombobox*s=BM_COMBOBOX(o);if(s->scope&&s->notify_id)g_signal_handler_disconnect(s->scope,s->notify_id);g_clear_object(&s->scope);G_OBJECT_CLASS(bm_combobox_parent_class)->dispose(o);}static void finalize(GObject*o){BmCombobox*s=BM_COMBOBOX(o);g_ptr_array_unref(s->values);g_free(s->label);g_free(s->value);g_free(s->error);g_free(s->custom_class);G_OBJECT_CLASS(bm_combobox_parent_class)->finalize(o);}
static void bm_combobox_class_init(BmComboboxClass*k){GObjectClass*c=G_OBJECT_CLASS(k);c->get_property=get_prop;c->set_property=set_prop;c->dispose=dispose;c->finalize=finalize;properties[PROP_LABEL]=g_param_spec_string("label",NULL,NULL,NULL,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);properties[PROP_VALUE]=g_param_spec_string("value",NULL,NULL,NULL,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);properties[PROP_REQUIRED]=g_param_spec_boolean("required",NULL,NULL,FALSE,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);properties[PROP_ERROR]=g_param_spec_string("error",NULL,NULL,NULL,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);properties[PROP_VALID]=g_param_spec_boolean("valid",NULL,NULL,TRUE,G_PARAM_READABLE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);properties[PROP_THEME_SCOPE]=g_param_spec_object("theme-scope",NULL,NULL,BM_TYPE_THEME_SCOPE,G_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY|G_PARAM_STATIC_STRINGS);g_object_class_install_properties(c,N_PROPERTIES,properties);signals[SIGNAL_CHANGED]=g_signal_new("changed",G_TYPE_FROM_CLASS(k),G_SIGNAL_RUN_LAST,0,NULL,NULL,NULL,G_TYPE_NONE,2,G_TYPE_STRING,G_TYPE_STRING);}
static void bm_combobox_init(BmCombobox*s){gtk_orientable_set_orientation(GTK_ORIENTABLE(s),GTK_ORIENTATION_VERTICAL);gtk_box_set_spacing(GTK_BOX(s),6);gtk_widget_add_css_class(GTK_WIDGET(s),"bm-combobox");s->label_widget=GTK_LABEL(gtk_label_new(NULL));gtk_label_set_xalign(s->label_widget,0);s->labels=gtk_string_list_new(NULL);s->values=g_ptr_array_new_with_free_func(g_free);s->control=GTK_DROP_DOWN(gtk_drop_down_new(G_LIST_MODEL(s->labels),NULL));gtk_drop_down_set_enable_search(s->control,TRUE);gtk_drop_down_set_selected(s->control,GTK_INVALID_LIST_POSITION);s->error_widget=GTK_LABEL(gtk_label_new(NULL));gtk_label_set_xalign(s->error_widget,0);gtk_widget_add_css_class(GTK_WIDGET(s->error_widget),"bm-combobox-error");gtk_widget_set_visible(GTK_WIDGET(s->error_widget),FALSE);gtk_box_append(GTK_BOX(s),GTK_WIDGET(s->label_widget));gtk_box_append(GTK_BOX(s),GTK_WIDGET(s->control));gtk_box_append(GTK_BOX(s),GTK_WIDGET(s->error_widget));g_signal_connect(s->control,"notify::selected",G_CALLBACK(on_selected),s);s->valid=TRUE;}
GtkWidget*bm_combobox_new(const char*l){return g_object_new(BM_TYPE_COMBOBOX,"label",l,NULL);}
