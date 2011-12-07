/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* test-gck-enumerator.c - the GObject PKCS#11 wrapper library

   Copyright (C) 2011 Collabora Ltd.

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

   Author: Stef Walter <stefw@collabora.co.uk>
*/

#include "config.h"

#include "gck/gck.h"
#include "gck/gck-mock.h"
#include "gck/gck-private.h"
#include "gck/gck-test.h"

#include "mock-interaction.h"

#include "egg/egg-testing.h"

#include <glib.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	GList *modules;
	GckModule *module;
} Test;

static void
setup (Test *test, gconstpointer unused)
{
	GError *err = NULL;

	/* Successful load */
	test->module = gck_module_initialize (BUILDDIR "/.libs/libmock-test-module.so", NULL, &err);
	g_assert_no_error (err);
	g_assert (GCK_IS_MODULE (test->module));

	test->modules = g_list_append (NULL, g_object_ref (test->module));
}

static void
teardown (Test *test, gconstpointer unused)
{
	gck_list_unref_free (test->modules);

	g_object_unref (test->module);
	egg_assert_not_object (test->module);

	g_thread_pool_stop_unused_threads ();
}

static void
test_create (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GType object_type;
	GckEnumerator *en;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	g_object_get (en, "object-type", &object_type, NULL);
	g_assert (object_type == GCK_TYPE_OBJECT);

	g_object_unref (en);
}

static void
test_create_slots (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GckEnumerator *en;
	GList *slots;

	uri_data = gck_uri_data_new ();
	slots = gck_module_get_slots (test->module, FALSE);
	en = _gck_enumerator_new_for_slots (slots, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));
	g_object_unref (en);
	gck_list_unref_free (slots);
}

static void
test_next (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GckObject *obj;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert (GCK_IS_OBJECT (obj));

	g_object_unref (obj);
	g_object_unref (en);
}

static void
test_next_slots (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GList *slots = NULL;
	GckEnumerator *en;
	GckObject *obj;

	uri_data = gck_uri_data_new ();
	slots = gck_module_get_slots (test->module, FALSE);
	en = _gck_enumerator_new_for_slots (slots, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert (GCK_IS_OBJECT (obj));

	g_object_unref (obj);
	g_object_unref (en);
	gck_list_unref_free (slots);
}

static void
test_next_and_resume (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GckObject *obj, *obj2;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert_no_error (error);
	g_assert (GCK_IS_OBJECT (obj));

	obj2 = gck_enumerator_next (en, NULL, &error);
	g_assert_no_error (error);
	g_assert (GCK_IS_OBJECT (obj2));

	g_assert (!gck_object_equal (obj, obj2));

	g_object_unref (obj);
	g_object_unref (obj2);
	g_object_unref (en);
}

static void
test_next_n (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GList *objects, *l;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	objects = gck_enumerator_next_n (en, -1, NULL, &error);
	g_assert_no_error (error);
	g_assert_cmpint (g_list_length (objects), ==, 5);
	for (l = objects; l; l = g_list_next (l))
		g_assert (GCK_IS_OBJECT (l->data));

	gck_list_unref_free (objects);
	g_object_unref (en);
}

static void
fetch_async_result (GObject *source, GAsyncResult *result, gpointer user_data)
{
	*((GAsyncResult**)user_data) = result;
	g_object_ref (result);
	egg_test_wait_stop ();
}

static void
test_next_async (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GAsyncResult *result = NULL;
	GError *error = NULL;
	GckEnumerator *en;
	GList *objects, *l;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	gck_enumerator_next_async (en, -1, NULL, fetch_async_result, &result);
	egg_test_wait_until (500);
	g_assert (result);

	objects = gck_enumerator_next_finish (en, result, &error);
	g_assert_no_error (error);
	g_assert_cmpint (g_list_length (objects), ==, 5);
	for (l = objects; l; l = g_list_next (l))
		g_assert (GCK_IS_OBJECT (l->data));

	g_object_unref (result);
	gck_list_unref_free (objects);
	g_object_unref (en);
}


static void
test_enumerate_session (Test *test,
                        gconstpointer unused)
{
	GckBuilder builder = GCK_BUILDER_INIT;
	GckEnumerator *en;
	GError *error = NULL;
	GckSession *session;
	GckObject *obj;
	GList *slots;

	slots = gck_module_get_slots (test->module, FALSE);
	g_assert (slots != NULL && GCK_IS_SLOT (slots->data));

	session = gck_session_open (slots->data, 0, NULL, NULL, &error);
	g_assert_no_error (error);

	en = gck_session_enumerate_objects (session, gck_builder_end (&builder));
	g_assert (GCK_IS_ENUMERATOR (en));

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert (GCK_IS_OBJECT (obj));

	g_object_unref (obj);
	g_object_unref (en);
	g_object_unref (session);
	gck_list_unref_free (slots);
}

static void
test_attribute_match (Test *test, gconstpointer unused)
{
	GckBuilder builder = GCK_BUILDER_INIT;
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GList *objects;

	uri_data = gck_uri_data_new ();
	gck_builder_add_string (&builder, CKA_LABEL, "Private Capitalize Key");
	uri_data->attributes = gck_attributes_ref_sink (gck_builder_end (&builder));
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	objects = gck_enumerator_next_n (en, -1, NULL, &error);
	g_assert_no_error (error);
	g_assert_cmpint (g_list_length (objects), ==, 1);
	g_assert (GCK_IS_OBJECT (objects->data));

	gck_list_unref_free (objects);
	g_object_unref (en);
}

static void
test_authenticate_interaction (Test *test,
                               gconstpointer unused)
{
	GTlsInteraction *interaction;
	GTlsInteraction *check;
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GckObject *obj;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, GCK_SESSION_LOGIN_USER, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	interaction = mock_interaction_new ("booo");
	g_object_set (en, "interaction", interaction, NULL);

	check = NULL;
	g_object_get (en, "interaction", &check, NULL);
	g_assert (interaction == check);
	g_object_unref (interaction);
	g_object_unref (check);

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert (GCK_IS_OBJECT (obj));

	g_object_unref (obj);
	g_object_unref (en);

	egg_assert_not_object (en);
	egg_assert_not_object (obj);
	egg_assert_not_object (interaction);
}

static gboolean
on_authenticate_token (GckModule *module,
                       GckSlot *slot,
                       gchar *label,
                       gchar **password,
                       gpointer unused)
{
	g_assert (unused == GUINT_TO_POINTER (35));
	g_assert (password != NULL);
	g_assert (*password == NULL);
	g_assert (GCK_IS_MODULE (module));
	g_assert (GCK_IS_SLOT (slot));

	*password = g_strdup ("booo");
	return TRUE;
}

static void
test_authenticate_compat (Test *test,
                          gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GckObject *obj;
	gulong sig;

	sig = g_signal_connect (test->modules->data, "authenticate-slot",
	                        G_CALLBACK (on_authenticate_token), GUINT_TO_POINTER (35));

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, GCK_SESSION_LOGIN_USER, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	obj = gck_enumerator_next (en, NULL, &error);
	g_assert (GCK_IS_OBJECT (obj));

	g_object_unref (obj);
	g_object_unref (en);

	g_signal_handler_disconnect (test->modules->data, sig);

	egg_assert_not_object (obj);
	egg_assert_not_object (en);
}

static void
test_token_match (Test *test, gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GList *objects;

	uri_data = gck_uri_data_new ();
	uri_data->token_info = g_new0 (GckTokenInfo, 1);
	uri_data->token_info->label = g_strdup ("Invalid token name");
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_assert (GCK_IS_ENUMERATOR (en));

	objects = gck_enumerator_next_n (en, -1, NULL, &error);
	g_assert_cmpint (g_list_length (objects), ==, 0);
	g_assert (error == NULL);

	gck_list_unref_free (objects);
	g_object_unref (en);
}

enum {
	PROP_0,
	PROP_ATTRIBUTES,
};

static const gulong mock_attribute_types[] = {
	CKA_CLASS,
	CKA_ID,
	CKA_LABEL,
};

typedef struct {
	GckObject parent;
	GckAttributes *attrs;
} MockObject;

typedef struct {
	GckObjectClass parent;
} MockObjectClass;

GType mock_object_get_type (void) G_GNUC_CONST;
static void mock_object_attributes_init (GckObjectAttributesIface *iface);

#define MOCK_TYPE_OBJECT     (mock_object_get_type())
#define MOCK_OBJECT(obj)     (G_TYPE_CHECK_INSTANCE_CAST((obj), MOCK_TYPE_OBJECT, MockObject))
#define MOCK_IS_OBJECT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE((obj), MOCK_TYPE_OBJECT))

G_DEFINE_TYPE_WITH_CODE (MockObject, mock_object, GCK_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GCK_TYPE_OBJECT_ATTRIBUTES,
                                                mock_object_attributes_init);
);

static void
mock_object_init (MockObject *self)
{

}

static void
mock_object_get_property (GObject *obj,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec)
{
	MockObject *self = (MockObject *)obj;

	switch (prop_id) {
	case PROP_ATTRIBUTES:
		g_value_set_boxed (value, self->attrs);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
		break;
	}
}

static void
mock_object_set_property (GObject *obj,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
	MockObject *self = (MockObject *)obj;

	switch (prop_id) {
	case PROP_ATTRIBUTES:
		g_assert (self->attrs == NULL);
		self->attrs = g_value_dup_boxed (value);
		g_assert (self->attrs != NULL);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
		break;
	}
}

static void
mock_object_finalize (GObject *obj)
{
	MockObject *self = (MockObject *)obj;
	gck_attributes_unref (self->attrs);
	G_OBJECT_CLASS (mock_object_parent_class)->finalize (obj);
}

static void
mock_object_class_init (MockObjectClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = mock_object_get_property;
	gobject_class->set_property = mock_object_set_property;
	gobject_class->finalize = mock_object_finalize;

	g_object_class_override_property (gobject_class, PROP_ATTRIBUTES, "attributes");
}

static void
mock_object_attributes_init (GckObjectAttributesIface *iface)
{
	iface->attribute_types = mock_attribute_types;
	iface->n_attribute_types = G_N_ELEMENTS (mock_attribute_types);
}

static void
test_attribute_get (Test *test,
                    gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	GList *objects, *l;
	MockObject *mock;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_object_set (en, "object-type", mock_object_get_type (), NULL);

	objects = gck_enumerator_next_n (en, -1, NULL, &error);
	g_assert_no_error (error);
	g_assert_cmpint (g_list_length (objects), ==, 5);

	for (l = objects; l != NULL; l = g_list_next (l)) {
		mock = l->data;
		g_assert (G_TYPE_CHECK_INSTANCE_TYPE (mock, mock_object_get_type ()));
		g_assert (mock->attrs != NULL);
	}

	gck_list_unref_free (objects);
	g_object_unref (en);
}

static void
test_attribute_get_one_at_a_time (Test *test,
                                  gconstpointer unused)
{
	GckUriData *uri_data;
	GError *error = NULL;
	GckEnumerator *en;
	MockObject *mock;
	GckObject *object;

	uri_data = gck_uri_data_new ();
	en = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	g_object_set (en, "object-type", mock_object_get_type (), NULL);

	for (;;) {
		object = gck_enumerator_next (en, NULL, &error);
		g_assert_no_error (error);
		if (object == NULL)
			break;

		g_assert (G_TYPE_CHECK_INSTANCE_TYPE (object, mock_object_get_type ()));
		mock = (MockObject *)object;
		g_assert (mock->attrs != NULL);
		g_object_unref (object);
	}

	g_object_unref (en);
}

static void
test_chained (Test *test,
              gconstpointer unused)
{
	GckBuilder builder = GCK_BUILDER_INIT;
	GckEnumerator *one;
	GckEnumerator *two;
	GckEnumerator *three;
	GckUriData *uri_data;
	GError *error = NULL;
	GList *objects;

	uri_data = gck_uri_data_new ();
	gck_builder_add_ulong (&builder, CKA_CLASS, CKO_PUBLIC_KEY);
	uri_data->attributes = gck_attributes_ref_sink (gck_builder_end (&builder));
	one = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);

	uri_data = gck_uri_data_new ();
	gck_builder_add_ulong (&builder, CKA_CLASS, CKO_PRIVATE_KEY);
	uri_data->attributes = gck_attributes_ref_sink (gck_builder_end (&builder));
	two = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	gck_enumerator_set_chained (one, two);

	uri_data = gck_uri_data_new ();
	gck_builder_add_ulong (&builder, CKA_CLASS, CKO_DATA);
	uri_data->attributes = gck_attributes_ref_sink (gck_builder_end (&builder));
	three = _gck_enumerator_new_for_modules (test->modules, 0, uri_data);
	gck_enumerator_set_chained (two, three);

	g_object_unref (two);
	g_object_unref (three);

	objects = gck_enumerator_next_n (one, -1, NULL, &error);
	g_assert_no_error (error);
	g_assert_cmpint (g_list_length (objects), ==, 5);

	gck_list_unref_free (objects);
	g_object_unref (one);
}

int
main (int argc, char **argv)
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_set_prgname ("test-gck-enumerator");

	g_test_add ("/gck/enumerator/create", Test, NULL, setup, test_create, teardown);
	g_test_add ("/gck/enumerator/create_slots", Test, NULL, setup, test_create_slots, teardown);
	g_test_add ("/gck/enumerator/next", Test, NULL, setup, test_next, teardown);
	g_test_add ("/gck/enumerator/next_slots", Test, NULL, setup, test_next_slots, teardown);
	g_test_add ("/gck/enumerator/next_and_resume", Test, NULL, setup, test_next_and_resume, teardown);
	g_test_add ("/gck/enumerator/next_n", Test, NULL, setup, test_next_n, teardown);
	g_test_add ("/gck/enumerator/next_async", Test, NULL, setup, test_next_async, teardown);
	g_test_add ("/gck/enumerator/session", Test, NULL, setup, test_enumerate_session, teardown);
	g_test_add ("/gck/enumerator/authenticate-interaction", Test, NULL, setup, test_authenticate_interaction, teardown);
	g_test_add ("/gck/enumerator/authenticate-compat", Test, NULL, setup, test_authenticate_compat, teardown);
	g_test_add ("/gck/enumerator/attribute_match", Test, NULL, setup, test_attribute_match, teardown);
	g_test_add ("/gck/enumerator/token_match", Test, NULL, setup, test_token_match, teardown);
	g_test_add ("/gck/enumerator/attribute_get", Test, NULL, setup, test_attribute_get, teardown);
	g_test_add ("/gck/enumerator/attribute_get_one_at_a_time", Test, NULL, setup, test_attribute_get_one_at_a_time, teardown);
	g_test_add ("/gck/enumerator/chained", Test, NULL, setup, test_chained, teardown);

	return egg_tests_run_with_loop ();
}
