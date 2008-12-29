/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gp11.h - the GObject PKCS#11 wrapper library

   Copyright (C) 2008, Stefan Walter

   The Gnome Keyring Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Keyring Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Stef Walter <nielsen@memberwebs.com>
*/

#ifndef GP11_H_
#define GP11_H_

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "pkcs11.h"

G_BEGIN_DECLS

#define             GP11_VENDOR_CODE                        0x47503131 /* GP11 */

/* An error code which results from a failure to load the PKCS11 module */
#define             CKR_GP11_MODULE_PROBLEM                 (CKR_VENDOR_DEFINED | (GP11_VENDOR_CODE + 1))

#define             GP11_ERROR                              (gp11_get_error_quark ())

GQuark              gp11_get_error_quark                    (void);

void                gp11_list_unref_free                    (GList *reflist);

const gchar*        gp11_message_from_rv                    (CK_RV rv);

gchar*              gp11_string_from_chars                  (const guchar *data, gsize max);

typedef struct GP11Mechanism {
	gulong type;
	gpointer parameter;
	gulong n_parameter;
} GP11Mechanism;

typedef struct GP11Attribute {
	gulong type;
	guchar *value;
	gulong length;
} GP11Attribute;

/*
 * Used with var args in place of a length to denote that this type
 * of value follows.
 */
enum {
	GP11_BOOLEAN = -1,
	GP11_ULONG = -2,
	GP11_STRING = -3,
	GP11_DATE = -4
};

#define GP11_INVALID (gulong)-1

void                gp11_attribute_init                     (GP11Attribute *attr,
                                                             gulong attr_type,
                                                             gconstpointer value,
                                                             gsize length);

void                gp11_attribute_init_invalid             (GP11Attribute *attr,
                                                             gulong attr_type);

void                gp11_attribute_init_boolean             (GP11Attribute *attr,
                                                             gulong attr_type,
                                                             gboolean value);

void                gp11_attribute_init_date                (GP11Attribute *attr,
                                                             gulong attr_type,
                                                             const GDate *value);

void                gp11_attribute_init_ulong               (GP11Attribute *attr,
                                                             gulong attr_type,
                                                             gulong value);

void                gp11_attribute_init_string              (GP11Attribute *attr,
                                                             gulong attr_type,
                                                             const gchar *value);

void                gp11_attribute_init_copy                (GP11Attribute *dest,
                                                             GP11Attribute *src);

GP11Attribute*      gp11_attribute_new                      (gulong attr_type,
                                                             gpointer value,
                                                             gsize length);

GP11Attribute*      gp11_attribute_new_invalid              (gulong attr_type);

GP11Attribute*      gp11_attribute_new_boolean              (gulong attr_type,
                                                             gboolean value);

GP11Attribute*      gp11_attribute_new_date                 (gulong attr_type,
                                                             const GDate *value);

GP11Attribute*      gp11_attribute_new_ulong                (gulong attr_type,
                                                             gulong value);

GP11Attribute*      gp11_attribute_new_string               (gulong attr_type,
                                                             const gchar *value);

gboolean            gp11_attribute_is_invalid               (GP11Attribute *attr);

gboolean            gp11_attribute_get_boolean              (GP11Attribute *attr);

gulong              gp11_attribute_get_ulong                (GP11Attribute *attr);

gchar*              gp11_attribute_get_string               (GP11Attribute *attr);

void                gp11_attribute_get_date                 (GP11Attribute *attr,
                                                             GDate* value);

GP11Attribute*      gp11_attribute_dup                      (GP11Attribute *attr);

void                gp11_attribute_clear                    (GP11Attribute *attr);

void                gp11_attribute_free                     (GP11Attribute *attr);


typedef struct _GP11Attributes GP11Attributes;

#define             GP11_TYPE_ATTRIBUTES                    (gp11_attributes_get_boxed_type ())

GType               gp11_attributes_get_boxed_type          (void) G_GNUC_CONST;

GP11Attributes*     gp11_attributes_new                     (void);

GP11Attributes*     gp11_attributes_newv                    (gulong attr_type, ...);

GP11Attributes*     gp11_attributes_new_valist              (va_list va);

void                gp11_attributes_set_immutable           (GP11Attributes *attrs);

gboolean            gp11_attributes_is_immutable            (GP11Attributes *attrs);

GP11Attribute*      gp11_attributes_at                      (GP11Attributes *attrs,
                                                             guint index);

void                gp11_attributes_add                     (GP11Attributes *attrs,
                                                             GP11Attribute *attr);

void                gp11_attributes_add_data                (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gconstpointer value,
                                                             gsize length);

void                gp11_attributes_add_invalid             (GP11Attributes *attrs,
                                                             gulong attr_type);

void                gp11_attributes_add_boolean             (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gboolean value);

void                gp11_attributes_add_string              (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             const gchar *string);

void                gp11_attributes_add_date                (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             const GDate *date);

void                gp11_attributes_add_ulong               (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gulong value);

GP11Attribute*      gp11_attributes_find                    (GP11Attributes *attrs,
                                                             gulong attr_type);

gboolean            gp11_attributes_find_boolean            (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gboolean *value);

gboolean            gp11_attributes_find_ulong              (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gulong *value);

gboolean            gp11_attributes_find_string             (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             gchar **value);

gboolean            gp11_attributes_find_date               (GP11Attributes *attrs,
                                                             gulong attr_type,
                                                             GDate *value);

gulong              gp11_attributes_count                   (GP11Attributes *attrs);

GP11Attributes*     gp11_attributes_ref                     (GP11Attributes *attrs);

void                gp11_attributes_unref                   (GP11Attributes *attrs);

/* -------------------------------------------------------------------------
 * FORWARDS
 */
typedef struct _GP11Slot GP11Slot;
typedef struct _GP11Module GP11Module;
typedef struct _GP11Session GP11Session;
typedef struct _GP11Object GP11Object;

/* -------------------------------------------------------------------------
 * MODULE
 */

typedef struct _GP11ModuleInfo {
	guint8 pkcs11_version_major;
	guint8 pkcs11_version_minor;

	gchar *manufacturer_id;
	gulong flags;

	gchar *library_description;
	guint8 library_version_major;
	guint8 library_version_minor;
} GP11ModuleInfo;

void                gp11_module_info_free                   (GP11ModuleInfo *module_info);

#define GP11_TYPE_MODULE             (gp11_module_get_type())
#define GP11_MODULE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GP11_TYPE_MODULE, GP11Module))
#define GP11_MODULE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GP11_TYPE_MODULE, GP11Module))
#define GP11_IS_MODULE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GP11_TYPE_MODULE))
#define GP11_IS_MODULE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GP11_TYPE_MODULE))
#define GP11_MODULE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GP11_TYPE_MODULE, GP11ModuleClass))

typedef struct _GP11ModuleClass GP11ModuleClass;

struct _GP11Module {
	GObject parent;
	gpointer reserved[4];
};

struct _GP11ModuleClass {
	GObjectClass parent;
	gpointer reserved[8];
};

GType                 gp11_module_get_type                    (void) G_GNUC_CONST;

GP11Module*           gp11_module_initialize                  (const gchar *path,
                                                               gpointer reserved,
                                                               GError **err);

const gchar*          gp11_module_get_path                    (GP11Module *self);

CK_FUNCTION_LIST_PTR  gp11_module_get_function_list           (GP11Module *self);

GP11ModuleInfo*       gp11_module_get_info                    (GP11Module *self);

GList*                gp11_module_get_slots                   (GP11Module *self,
                                                               gboolean token_present);

enum {
	GP11_IS_STRING = -1,
	GP11_IS_BOOLEAN = -2,
	GP11_IS_DATE = -3,
	GP11_IS_ULONG = -4
};

/* ------------------------------------------------------------------------
 * SLOT
 */

typedef struct _GP11SlotInfo {
	gchar *slot_description;
	gchar *manufacturer_id;
	gulong flags;
	guint8 hardware_version_major;
	guint8 hardware_version_minor;
	guint8 firmware_version_major;
	guint8 firmware_version_minor;
} GP11SlotInfo;

void                gp11_slot_info_free                      (GP11SlotInfo *slot_info);

typedef struct _GP11TokenInfo {
	gchar *label;
	gchar *manufacturer_id;
	gchar *model;
	gchar *serial_number;
	gulong flags;
	glong max_session_count;
	glong session_count;
	glong max_rw_session_count;
	glong rw_session_count;
	glong max_pin_len;
	glong min_pin_len;
	glong total_public_memory;
	glong free_public_memory;
	glong total_private_memory;
	glong free_private_memory;
	guint8 hardware_version_major;
	guint8 hardware_version_minor;
	guint8 firmware_version_major;
	guint8 firmware_version_minor;
	gint64 utc_time;
} GP11TokenInfo;

void                gp11_token_info_free                    (GP11TokenInfo *token_info);

typedef struct _GP11MechanismInfo {
	gulong min_key_size;
	gulong max_key_size;
	gulong flags;
} GP11MechanismInfo;

void                gp11_mechanism_info_free                (GP11MechanismInfo *mech_info);

typedef GArray GP11Mechanisms;

#define gp11_mechanisms_length(a)  	((a)->len)

#define gp11_mechanisms_at(a, i) 	(g_array_index(a, CK_MECHANISM_TYPE, i))

#define gp11_mechanisms_free(a)         (g_array_free(a, TRUE))

#define GP11_TYPE_SLOT             (gp11_slot_get_type())
#define GP11_SLOT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GP11_TYPE_SLOT, GP11Slot))
#define GP11_SLOT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GP11_TYPE_SLOT, GP11Slot))
#define GP11_IS_SLOT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GP11_TYPE_SLOT))
#define GP11_IS_SLOT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GP11_TYPE_SLOT))
#define GP11_SLOT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GP11_TYPE_SLOT, GP11SlotClass))

typedef struct _GP11SlotClass GP11SlotClass;

struct _GP11Slot {
	GObject parent;
	gpointer reserved[4];
};

struct _GP11SlotClass {
	GObjectClass parent;

	gboolean (*authenticate_token) (GP11Slot *self, gchar *label, gchar **password);

	gboolean (*authenticate_object) (GP11Slot *self, GP11Object *object, gchar *label, gchar **password);

#ifdef UNIMPLEMENTED
	void (*slot_event) (GP11Slot *self);
#endif

	gpointer reserved[9];
};

GType               gp11_slot_get_type                      (void) G_GNUC_CONST;

GP11Module*         gp11_slot_get_module                    (GP11Slot *self);

CK_SLOT_ID          gp11_slot_get_handle                    (GP11Slot *self);

gboolean            gp11_slot_get_reuse_sessions            (GP11Slot *self);

void                gp11_slot_set_reuse_sessions            (GP11Slot *self,
                                                             gboolean reuse);

gboolean            gp11_slot_get_auto_login                (GP11Slot *self);

void                gp11_slot_set_auto_login                (GP11Slot *self,
                                                             gboolean auto_login);

gint                gp11_slot_get_max_pin_length            (GP11Slot *self);

GP11SlotInfo*       gp11_slot_get_info                      (GP11Slot *self);

GP11TokenInfo*      gp11_slot_get_token_info                (GP11Slot *self);

GP11Mechanisms*     gp11_slot_get_mechanisms                (GP11Slot *self);

GP11MechanismInfo*  gp11_slot_get_mechanism_info            (GP11Slot *self,
                                                             gulong mech_type);

#if UNIMPLEMENTED

gboolean            gp11_slot_init_token                    (GP11Slot *self,
                                                             const guchar *pin,
                                                             gsize length,
                                                             const gchar *label,
                                                             GError **err);


void                gp11_slot_init_token_async              (GP11Slot *self,
                                                             const guchar *pin,
                                                             gsize length,
                                                             const gchar *label,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_slot_init_token_finish             (GP11Slot *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#endif /* UNIMPLEMENTED */

GP11Session*        gp11_slot_open_session                  (GP11Slot *self,
                                                             gulong flags,
                                                             GError **err);

GP11Session*        gp11_slot_open_session_full             (GP11Slot *self,
                                                             gulong flags,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_slot_open_session_async            (GP11Slot *self,
                                                             gulong flags,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GP11Session*        gp11_slot_open_session_finish           (GP11Slot *self,
                                                    	     GAsyncResult *result,
                                                    	     GError **err);

/* ------------------------------------------------------------------------
 * SESSION
 */

typedef struct _GP11SessionInfo {
	gulong slot_id;
	gulong state;
	gulong flags;
	gulong device_error;
} GP11SessionInfo;

void                gp11_session_info_free                  (GP11SessionInfo *session_info);

#define GP11_TYPE_SESSION             (gp11_session_get_type())
#define GP11_SESSION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GP11_TYPE_SESSION, GP11Session))
#define GP11_SESSION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GP11_TYPE_SESSION, GP11Session))
#define GP11_IS_SESSION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GP11_TYPE_SESSION))
#define GP11_IS_SESSION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GP11_TYPE_SESSION))
#define GP11_SESSION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GP11_TYPE_SESSION, GP11SessionClass))

typedef struct _GP11SessionClass GP11SessionClass;

struct _GP11Session {
	GObject parent;
	gpointer reserved[4];
};

struct _GP11SessionClass {
	GObjectClass parent;

	gboolean (*discard_handle) (GP11Session *session, CK_SESSION_HANDLE handle);

	gpointer reserved[8];
};

GType               gp11_session_get_type                   (void) G_GNUC_CONST;

GP11Session*        gp11_session_from_handle                (GP11Slot *slot, CK_SESSION_HANDLE handle);

GP11Module*         gp11_session_get_module                 (GP11Session *self);

GP11Slot*           gp11_session_get_slot                   (GP11Session *self);

CK_SESSION_HANDLE   gp11_session_get_handle                 (GP11Session *self);

CK_SESSION_HANDLE   gp11_session_steal_handle               (GP11Session *self);

GP11SessionInfo*    gp11_session_get_info                   (GP11Session *self);

#if UNIMPLEMENTED

gboolean            gp11_session_init_pin                   (GP11Session *self,
                                                             const guchar *pin,
                                                             gsize n_pin,
                                                             GError **err);

void                gp11_session_init_pin_async             (GP11Session *self,
                                                             const guchar *pin,
                                                             gsize n_pin,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_init_pin_finish            (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

gboolean            gp11_session_set_pin                    (GP11Session *self,
                                                             const guchar *old_pin,
                                                             gsize n_old_pin,
                                                             const guchar *new_pin,
                                                             gsize n_new_pin,
                                                             GError **err);

void                gp11_session_set_pin_async              (GP11Session *self,
                                                             const guchar *old_pin,
                                                             gsize n_old_pin,
                                                             const guchar *new_pin,
                                                             gsize n_new_pin,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_set_pin_finish             (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

guchar*             gp11_session_get_operation_state        (GP11Session *self,
                                                             gsize *n_result,
                                                             GError **err);

void                gp11_session_get_operation_state_async  (GP11Session *self,
                                                             gsize *n_result,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

guchar*             gp11_session_get_operation_state_finish (GP11Session *self,
                                                             GAsyncResult *result,
                                                             gsize *n_result,
                                                             GError **err);

gboolean            gp11_session_set_operation_state        (GP11Session *self,
                                                             const guchar *state,
                                                             gsize n_state,
                                                             GError **err);

void                gp11_session_set_operation_state_async  (GP11Session *self,
                                                             const guchar *state,
                                                             gsize n_state,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_set_operation_state_finish (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#endif /* UNIMPLEMENTED */

gboolean            gp11_session_login                      (GP11Session *self,
                                                             gulong user_type,
                                                             const guchar *pin,
                                                             gsize n_pin,
                                                             GError **err);

gboolean            gp11_session_login_full                 (GP11Session *self,
                                                             gulong user_type,
                                                             const guchar *pin,
                                                             gsize n_pin,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_session_login_async                (GP11Session *self,
                                                             gulong user_type,
                                                             const guchar *pin,
                                                             gsize n_pin,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_login_finish               (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

gboolean            gp11_session_logout                     (GP11Session *self,
                                                             GError **err);

gboolean            gp11_session_logout_full                (GP11Session *self,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_session_logout_async               (GP11Session *self,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_logout_finish              (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

GP11Object*         gp11_session_create_object              (GP11Session *self,
                                                             GError **err,
                                                             ...);

GP11Object*         gp11_session_create_object_full         (GP11Session *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_session_create_object_async        (GP11Session *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GP11Object*         gp11_session_create_object_finish       (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

GList*              gp11_session_find_objects               (GP11Session *self,
                                                             GError **err,
                                                             ...);

GList*              gp11_session_find_objects_full          (GP11Session *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_session_find_objects_async         (GP11Session *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GList*              gp11_session_find_objects_finish        (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#if UNIMPLEMENTED

GP11Object*         gp11_session_generate_key               (GP11Session *self,
                                                             GP11Mechanism *mechanism,
                                                             GError **err,
                                                             ...);

void                gp11_session_generate_key_async         (GP11Session *self,
                                                             GP11Mechanism *mechanism,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data,
                                                             ...);

GP11Object*         gp11_session_generate_key_finish        (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err,
                                                             ...);

gboolean            gp11_session_generate_key_pair          (GP11Session *self,
                                                             GP11Mechanism *mechanism,
                                                             GP11Object **public_key,
                                                             GP11Object **private_key,
                                                             GError **err,
                                                             ...);

void                gp11_session_generate_key_pair_async    (GP11Session *self,
                                                             GP11Mechanism *mechanism,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data,
                                                             ...);

gboolean            gp11_session_generate_key_pair_finish   (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GP11Object **public_key,
                                                             GP11Object **private_key,
                                                             GError **err,
                                                             ...);

gboolean            gp11_session_seed_random                (GP11Session *self,
                                                             const guchar *seed,
                                                             gsize n_seed,
                                                             GError **err);

void                gp11_session_seed_random_async          (GP11Session *self,
                                                             const guchar *seed,
                                                             gsize n_seed,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_session_seed_random_finish         (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

guchar*             gp11_session_generate_random            (GP11Session *self,
                                                             gsize n_random,
                                                             GError **err);

void                gp11_session_generate_random_async      (GP11Session *self,
                                                             gsize n_random,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

guchar*             gp11_session_generate_random_finish     (GP11Session *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#endif /* UNIMPLEMENTED */

guchar*             gp11_session_encrypt                     (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_encrypt_full                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_encrypt_async               (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_encrypt_finish              (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

#if UNIMPLEMENTED

GP11Processor*      gp11_session_batch_encrypt               (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_encrypt_async         (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_encrypt_finish        (GP11Session *self,
                                                              GP11Object *key,
                                                              GAsyncResult *result,
                                                              GError **err);

#endif /* UNIMPLEMENTED */

guchar*             gp11_session_decrypt                     (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_decrypt_full                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_decrypt_async               (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_decrypt_finish              (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

#if UNIMPLEMENTED

GP11Processor*      gp11_session_batch_decrypt               (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_decrypt_async         (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_decrypt_finish        (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

guchar*             gp11_session_digest                      (GP11Session *self,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_digest_full                 (GP11Session *self,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_digest_async                (GP11Session *self,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_digest_finish               (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

GP11Processor*      gp11_session_batch_digest	             (GP11Session *self,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_digest_async          (GP11Session *self,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_digest_finish         (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

GP11Processor*      gp11_session_batch_digest_encrypt        (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *digest_mech,
                                                              GP11Mechanism *encrypt_mech,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_digest_encrypt_async  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *digest_mech,
                                                              GP11Mechanism *encrypt_mech,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_digest_encrypt_finish (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

GP11Processor*      gp11_session_batch_digest_decrypt        (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *digest_mech,
                                                              GP11Mechanism *decrypt_mech,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_digest_decrypt_async  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *digest_mech,
                                                              GP11Mechanism *decrypt_mech,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_digest_decrypt_finish (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

GP11Processor*      gp11_session_batch_decrypt_verify        (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *decrypt_mech,
                                                              GP11Mechanism *verify_mech,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_decrypt_verify_async  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *decrypt_mech,
                                                              GP11Mechanism *verify_mech,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_decrypt_verify_finish (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

#endif /* UNIMPLEMENTED */

guchar*             gp11_session_sign                        (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_sign_full                   (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_sign_async                  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_sign_finish                 (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

#if UNIMPLEMENTED

GP11Processor*      gp11_session_batch_sign                  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_sign_async            (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_sign_finish           (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

GP11Processor*      gp11_session_batch_sign_encrypt          (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *sign_mech,
                                                              GP11Mechanism *encrypt_mech,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_sign_encrypt_async    (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *sign_mechanism,
                                                              GP11Mechanism *encrypt_mech,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GP11Processor*      gp11_session_batch_sign_encrypt_finish   (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

guchar*             gp11_session_sign_recover                (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_sign_recover_full           (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_sign_recover_async          (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_sign_recover_finish         (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

#endif /* UNIMPLEMENTED */

gboolean            gp11_session_verify                      (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              const guchar *signature,
                                                              gsize n_signature,
                                                              GError **err);

gboolean            gp11_session_verify_full                 (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              const guchar *signature,
                                                              gsize n_signature,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_verify_async                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mechanism,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              const guchar *signature,
                                                              gsize n_signature,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

gboolean            gp11_session_verify_finish               (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

#if UNIMPLEMENTED

GkrProcessor*       gp11_session_batch_verify                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_batch_verify_async          (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

GkrProcessor*       gp11_session_batch_verify_finish         (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

guchar*             gp11_session_verify_recover              (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_verify_recover_full         (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_verify_recover_async        (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_verify_recover_finish       (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_wrap                        (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              GP11Object *wrapped_key,
                                                              gsize *n_result,
                                                              GError **err);

guchar*             gp11_session_wrap                        (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GP11Object *wrapped_key,
                                                              gsize *n_result,
                                                              GCancellable *cancellable,
                                                              GError **err);

void                gp11_session_wrap_async                  (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GP11Object *wrapped_key,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);

guchar*             gp11_session_wrap_finish                 (GP11Session *self,
                                                              GAsyncResult *result,
                                                              gsize *n_result,
                                                              GError **err);

GP11Object*         gp11_session_unwrap                      (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GError **err,
                                                              ...);

GP11Object*         gp11_session_unwrap                      (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GError **err,
                                                              ...);

void                gp11_session_unwrap_async                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              const guchar *input,
                                                              gsize n_input,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);
                                                              ...);

GP11Object*         gp11_session_unwrap_finish               (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

GP11Object*         gp11_session_derive                      (GP11Session *self,
                                                              GP11Object *key,
                                                              gulong mech_type,
                                                              GError **err,
                                                              ...);

GP11Object*         gp11_session_derive_full                 (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GError **err,
                                                              ...);

void                gp11_session_derive_async                (GP11Session *self,
                                                              GP11Object *key,
                                                              GP11Mechanism *mech_args,
                                                              GCancellable *cancellable,
                                                              GAsyncReadyCallback callback,
                                                              gpointer user_data);
                                                              ...);

GP11Object*         gp11_session_derive_finish               (GP11Session *self,
                                                              GAsyncResult *result,
                                                              GError **err);

#endif /* UNIMPLEMENTED */


/* ------------------------------------------------------------------------
 * OBJECT
 */

#define GP11_TYPE_OBJECT             (gp11_object_get_type())
#define GP11_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GP11_TYPE_OBJECT, GP11Object))
#define GP11_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GP11_TYPE_OBJECT, GP11Object))
#define GP11_IS_OBJECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GP11_TYPE_OBJECT))
#define GP11_IS_OBJECT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GP11_TYPE_OBJECT))
#define GP11_OBJECT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GP11_TYPE_OBJECT, GP11ObjectClass))

typedef struct _GP11ObjectClass GP11ObjectClass;

struct _GP11Object {
	GObject parent;
	gpointer reserved[4];
};

struct _GP11ObjectClass {
	GObjectClass parent;
	gpointer reserved[8];
};

GType               gp11_object_get_type                    (void) G_GNUC_CONST;

GP11Object*         gp11_object_from_handle                 (GP11Slot *slot,
                                                             CK_OBJECT_HANDLE handle);

GList*              gp11_objects_from_handle_array          (GP11Slot *slot,
                                                             const GP11Attribute *attr);

GP11Module*         gp11_object_get_module                  (GP11Object *self);

GP11Slot*           gp11_object_get_slot                    (GP11Object *self);

CK_OBJECT_HANDLE    gp11_object_get_handle                  (GP11Object *self);

GP11Session*        gp11_object_get_session                 (GP11Object *self);

void                gp11_object_set_session                 (GP11Object *self,
                                                             GP11Session *session);

#ifdef UNIMPLEMENTED

GP11Object*         gp11_object_copy                        (GP11Object *self,
                                                             GError **err);

GP11Object*         gp11_object_copy_full                   (GP11Object *self,
                                                             GP11Attributes *additional,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_copy_async                  (GP11Object *self,
                                                             GP11Attributes *additional,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GP11Object*         gp11_object_copy_finish                 (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#endif /* UNIMPLEMENTED */

gboolean            gp11_object_destroy                     (GP11Object *self,
                                                             GError **err);

gboolean            gp11_object_destroy_full                (GP11Object *self,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_destroy_async               (GP11Object *self,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_object_destroy_finish              (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#if UNIMPLEMENTED

gssize              gp11_object_get_size                    (GP11Object *self,
                                                             GError **err);

gssize              gp11_object_get_size_full               (GP11Object *self,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_get_size_async              (GP11Object *self,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gssize              gp11_object_get_size_finish             (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);

#endif /* UNIMPLEMENTED */

gboolean            gp11_object_set                         (GP11Object *self,
                                                             GError **err,
                                                             ...);

gboolean            gp11_object_set_full                    (GP11Object *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_set_async                   (GP11Object *self,
                                                             GP11Attributes *attrs,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

gboolean            gp11_object_set_finish                  (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);

GP11Attributes*     gp11_object_get                         (GP11Object *self,
                                                             GError **err,
                                                             ...);

GP11Attributes*     gp11_object_get_full                    (GP11Object *self,
                                                             const gulong *attr_types,
                                                             gsize n_attr_types,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_get_async                   (GP11Object *self,
                                                             const gulong *attr_types,
                                                             gsize n_attr_types,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GP11Attributes*     gp11_object_get_finish                  (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);

GP11Attribute*      gp11_object_get_one                     (GP11Object *self,
                                                             gulong attr_type,
                                                             GError **err);

GP11Attribute*      gp11_object_get_one_full                (GP11Object *self,
                                                             gulong attr_type,
                                                             GCancellable *cancellable,
                                                             GError **err);

void                gp11_object_get_one_async               (GP11Object *self,
                                                             gulong attr_type,
                                                             GCancellable *cancellable,
                                                             GAsyncReadyCallback callback,
                                                             gpointer user_data);

GP11Attribute*      gp11_object_get_one_finish              (GP11Object *self,
                                                             GAsyncResult *result,
                                                             GError **err);


/* ----------------------------------------------------------------------
 * PROCESSOR
 */

#if UNIMPLEMENTED

guchar*             gp11_processor_step                    (GP11Processor *processor,
                                                            const guchar *input,
                                                            gsize n_input,
                                                            gsize *n_result,
                                                            GError **err);

void                gp11_processor_step_async              (GP11Processor *processor,
                                                            const guchar *input,
                                                            gsize n_input,
                                                            GAsyncReadyCallback callback,
                                                            gpointer user_data);

guchar*             gp11_processor_step_finish             (GP11Processor *processor,
                                                            GAsyncResult *result,
                                                            gsize *n_result,
                                                            GError **err);

guchar*             gp11_processor_close                   (GP11Processor *processor,
                                                            gsize *n_result,
                                                            GError **err);

guchar*             gp11_processor_close_async             (GP11Processor *processor,
                                                            GAsyncReadyCallback callback,
                                                            gpointer user_data);

guchar*             gp11_processor_close_finish            (GP11Processor *processor,
                                                            GAsyncResult *result,
                                                            gsize *n_result,
                                                            GError **err);

#endif /* UNIMPLEMENTED */

G_END_DECLS

#endif /*GP11_H_*/
