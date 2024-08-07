/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


#include <stdio.h>
#include <stdlib.h>

static void screen_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_SCREEN_LOADED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_OUT, 100, 5000, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_arc_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data1;
	int my_data = *((int *)lv_event_get_param(e));
	// sprintf(my_data1,"%d",my_data);
	// strcat(my_data1,"%");
	// lv_label_set_text_fmt(guider_ui.screen_label_12,"%s",my_data1);
	lv_arc_set_value(guider_ui.screen_arc_1,my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_label_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_label_1,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_label_5_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_label_5,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_label_8_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data8 = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_label_8,"%s",my_data8);
		break;
	}
	default:
		break;
	}
}
static void screen_label_9_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_label_9,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_label_12_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_label_12,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen, screen_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_arc_1, screen_arc_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_label_1, screen_label_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_label_5, screen_label_5_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_label_8, screen_label_8_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_label_9, screen_label_9_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_label_12, screen_label_12_event_handler, LV_EVENT_ALL, ui);
}
static void screen_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_SCREEN_LOADED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.screen_1_del, setup_scr_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 100, 5000, false, false);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_12_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_12,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_14_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_14,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_15_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_15,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_16_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_16,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_17_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_17,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
static void screen_1_label_18_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		char * my_data = (char *)lv_event_get_param(e);
	lv_label_set_text_fmt(guider_ui.screen_1_label_18,"%s",my_data);
		break;
	}
	default:
		break;
	}
}
void events_init_screen_1(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_1, screen_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_12, screen_1_label_12_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_14, screen_1_label_14_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_15, screen_1_label_15_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_16, screen_1_label_16_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_17, screen_1_label_17_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_1_label_18, screen_1_label_18_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}
