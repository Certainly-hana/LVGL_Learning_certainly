#include "my_gui.h"
#include "lvgl.h"
#include "lcd.h"
#include "usart.h"	

#if 1
/**
平铺视图部件

**/

/*********
*正弦波定义
**********/
#define DISP_VER_RES 150
#define SIN_POINT_MAX 80
#define PI 3.1415926
//top time_view label 
lv_obj_t *label_left;


lv_obj_t *label_sl;
lv_obj_t *label_vol_view;

lv_obj_t *user;
lv_obj_t *passwd;
lv_obj_t *btn_login;
lv_obj_t *keyboard;


//btns from control
lv_obj_t *wifi_btn;
lv_obj_t *blue_btn;
lv_obj_t *other_btn;

lv_obj_t *label_down_vol;
lv_obj_t *label_down_sl;

uint8_t wifi_state_flag = 0;
uint8_t blue_state_flag = 0;
uint8_t other_state_flag = 0;


//pagedown win

lv_obj_t *wifi_win;
lv_obj_t *blue_win;
lv_obj_t *other_win;

lv_obj_t *win_wifi_label;
lv_obj_t *win_blue_label;
lv_obj_t *win_other_label;

lv_obj_t *win_wifi_btn;
lv_obj_t *win_blue_btn;
lv_obj_t *win_other_btn;


static lv_obj_t *label_temp;
//static lv_point_t  line_points[DISP_VER_RES]={0};
//static lv_point_t  line_points_humi[DISP_VER_RES]={0};

//static lv_obj_t *line_temp;
//static lv_obj_t *line_humi;
static lv_obj_t *temp_label;
static lv_obj_t *humi_label;

lv_obj_t *temp_chart;
lv_chart_series_t *temp_chart_line;
lv_chart_series_t *humi_chart_line;


//heart and spo2 chart
lv_obj_t *heart_chart;
lv_chart_series_t *heart_chart_line;
lv_chart_series_t *spo2_chart_line;



static lv_obj_t *tile4;
static lv_obj_t *down_page;

//downpage win
void pagedown_wifi_win_func(void);
void pagedown_blue_win_func(void);
void pagedown_other_win_func(void);


static const char *btns[] = { "Continue", "Close", "" };

//动画结构体
typedef struct _ui_anim_user_data_t {
    lv_obj_t * target;
    lv_img_dsc_t ** imgset;
    int32_t imgset_size;
    int32_t val;
} ui_anim_user_data_t;


//动画动作
void _ui_anim_callback_set_x(lv_anim_t * a, int32_t v)
{
    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    lv_obj_set_x(usr->target, v);
}

void _ui_anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    lv_obj_set_y(usr->target, v);
}


int32_t _ui_anim_callback_get_x(lv_anim_t * a)
{
    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    return lv_obj_get_x_aligned(usr->target);
}

int32_t _ui_anim_callback_get_y(lv_anim_t * a)
{
    ui_anim_user_data_t * usr = (ui_anim_user_data_t *)a->user_data;
    return lv_obj_get_y_aligned(usr->target);
}


void moveup_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 200);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, -320);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
//    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);
    lv_anim_start(&PropertyAnimation_0);
}



void movedown_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 200);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, 320);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
//    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);
    lv_anim_start(&PropertyAnimation_0);
}


//动画事件

void event_downpage(lv_event_t *ev)
{
    lv_event_code_t code = lv_event_get_code(ev);

    lv_obj_t *target = lv_event_get_target(ev);
    if(code == LV_EVENT_PRESSED)
    {
        movedown_Animation(down_page,1);
    }
}

void event_uppage(lv_event_t *ev)
{
    lv_event_code_t code = lv_event_get_code(ev);

    lv_obj_t *target = lv_event_get_target(ev);
    if(code == LV_EVENT_PRESSED)
    {
        moveup_Animation(down_page,1);
    }

}




//static void sin_timer(lv_timer_t *timer)
//{
//    lv_obj_t *line =timer->user_data;
//    static uint16_t i =0;
//    uint16_t j =0;
//    float hd = 0.0; //弧度
//    float fz = 0.0; //峰值

//    j = SIN_POINT_MAX / 2;
//    hd = PI / j;

//    //屏幕宽度
//    if(i>= DISP_VER_RES)
//    {
//        i = 0;
//    }
//    fz = j*sin(hd * i)+j;
//    line_points[i].y = (uint16_t)fz;
//    line_points[i].x = i;
//    printf("[%d,%d]\n",line_points[i].x,line_points[i].y);
//    lv_line_set_points(line,line_points,i);

//    i++;
//}





static void event_ca(lv_event_t *ev)
{
    lv_obj_t *target = lv_event_get_target(ev);
    lv_event_code_t code = lv_event_get_code(ev);

    if(code == LV_EVENT_FOCUSED)
    {
        lv_obj_clear_flag(keyboard,LV_OBJ_FLAG_HIDDEN);
        lv_keyboard_set_textarea(keyboard,target);
    }
    else if(target == btn_login)
    {
        if(!(strcmp(lv_textarea_get_text(user),"Admin") && strcmp(lv_textarea_get_text(passwd),"12345")))
        {

            lv_obj_t *msgbox = lv_msgbox_create( lv_scr_act(), "Notice", "Login Success!", btns, true );
            lv_obj_set_align(msgbox,LV_ALIGN_CENTER);

        }else
        {
            printf("LOGIN ERROR\n");
            lv_obj_t *msgbox = lv_msgbox_create( lv_scr_act(), "Notice", "Login Error!", btns, true );
            lv_obj_set_align(msgbox,LV_ALIGN_CENTER);
        }
    }



}
static void key_event(lv_event_t *ev)
{
    lv_event_code_t code = lv_event_get_code(ev);
    lv_obj_t *target = lv_event_get_target(ev);


    if(code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t id =lv_btnmatrix_get_selected_btn(target);     //获取键盘按钮索引
        const char *btn_txt = lv_btnmatrix_get_btn_text(target,id); //获取键盘按钮内容

        if(!strcmp(btn_txt,LV_SYMBOL_KEYBOARD))      //判断是不是键盘按钮被按下
        {
            if(lv_keyboard_get_mode(target) == LV_KEYBOARD_MODE_NUMBER)     //获取当前键盘模式，
                                                                            //    如果是数字模式就切换小写模式
            {
                lv_keyboard_set_mode(target,LV_KEYBOARD_MODE_TEXT_LOWER);   //切换小写字母模式
            }
            else{
              lv_keyboard_set_mode(target,LV_KEYBOARD_MODE_NUMBER);
            }
        }else if(id == 39)
        {
            lv_obj_add_flag(keyboard,LV_OBJ_FLAG_HIDDEN);
        }

    }

}


static void event_cb(lv_event_t *ev)
{
    lv_obj_t *target = lv_event_get_target(ev);

    int32_t vol = lv_slider_get_value(target);
    if(vol >= 60)
    {
        lv_label_set_text(label_sl,LV_SYMBOL_VOLUME_MAX);
        lv_obj_set_style_text_color(label_sl,lv_color_hex(0x66ccff),0);
        printf("vol value = [%d]\n",vol);
        lv_label_set_text_fmt(label_vol_view,"VOL:%d%%",vol);

    }else
    {
        lv_label_set_text(label_sl,LV_SYMBOL_VOLUME_MID);
        lv_obj_set_style_text_color(label_sl,lv_color_hex(0x3a404d),0);
        printf("vol value = [%d]\n",vol);
        lv_label_set_text_fmt(label_vol_view,"VOL:%d%%",vol);
    }

}

//downpage vol setting
void downpage_btn_event(lv_event_t *ev)
{

    lv_obj_t *target = lv_event_get_target(ev);

    uint8_t vol = (uint8_t)lv_slider_get_value(target);
    if(vol >= 60)
    {
        lv_label_set_text(label_down_sl,LV_SYMBOL_VOLUME_MAX);
        lv_obj_set_style_text_color(label_down_sl,lv_color_hex(0xffffff),0);
        printf("vol value = [%d]\n",vol);
        lv_label_set_text_fmt(label_down_vol,"VOL:%d%%",vol);
		LCD_SSD_BackLightSet(vol);
		LCD_DisplayOff();

    }else
    {
        lv_label_set_text(label_down_sl,LV_SYMBOL_VOLUME_MID);
        lv_obj_set_style_text_color(label_down_sl,lv_color_hex(0x000000),0);
        printf("vol value = [%d]\n",vol);
        lv_label_set_text_fmt(label_down_vol,"VOL:%d%%",vol);
		LCD_SSD_BackLightSet(vol);
		LCD_DisplayOn();
    }

}


void usart_blue_event(lv_event_t *ev)
{
//	lv_label_set_text_fmt(win_blue_label,"%s",g_usart3_buf);
//	
//	lv_label_set_text_fmt(win_other_label,"%s",g_usart2_buf);
//	
//	lv_label_set_text_fmt(win_wifi_label,"%s",g_usart1_buf);
	
	
}


//downpage control btn event
void downpage_control_event(lv_event_t *ev)
{
    lv_obj_t *target = lv_event_get_target(ev);
    lv_event_code_t code =lv_event_get_code(ev);
    if(target == wifi_btn)
    {
        if(code == LV_EVENT_CLICKED && wifi_state_flag == 0 )
        {

            lv_obj_set_style_bg_color(target,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_add_state(target,LV_STATE_CHECKED);
            wifi_state_flag=1;
            printf("wifi is open\n");


        }
        else if(code == LV_EVENT_CLICKED && wifi_state_flag == 1)
        {

            lv_obj_add_state(target,LV_STATE_DEFAULT);
            lv_obj_clear_state(target,LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
            printf("wifi is close \n");
            wifi_state_flag=0;

        }
		if(code == LV_EVENT_LONG_PRESSED)
        {
            pagedown_wifi_win_func();
        }


    }
     if(target == blue_btn)
    {
        if(code == LV_EVENT_CLICKED && blue_state_flag == 0 )
        {

            lv_obj_set_style_bg_color(target,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_add_state(target,LV_STATE_CHECKED);
            blue_state_flag=1;
            printf("blue is open\n");


        }
        else if(code == LV_EVENT_CLICKED && blue_state_flag == 1)
        {

            lv_obj_add_state(target,LV_STATE_DEFAULT);
            lv_obj_clear_state(target,LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(target,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
            printf("blue is close \n");
            blue_state_flag=0;

        }
		if(code == LV_EVENT_LONG_PRESSED)
        {
            pagedown_blue_win_func();
        }

    }
     if(target == other_btn)
    {
        if(code == LV_EVENT_CLICKED && other_state_flag == 0 )
        {

            lv_obj_set_style_bg_color(target,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_add_state(target,LV_STATE_CHECKED);
            other_state_flag=1;
            printf("other is open\n");


        }
        else if(code == LV_EVENT_CLICKED && other_state_flag == 1)
        {

            lv_obj_add_state(target,LV_STATE_DEFAULT);
            lv_obj_clear_state(target,LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(target,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
            printf("other is close \n");
            other_state_flag=0;

        }
		if(code == LV_EVENT_LONG_PRESSED)
        {
            pagedown_other_win_func();
        }

    }


}


void get_heart(uint8_t heart_value,uint8_t spo2_value)
{

	    lv_chart_set_next_value(heart_chart,heart_chart_line,(uint16_t)heart_value);
		lv_chart_set_next_value(heart_chart,spo2_chart_line,(uint16_t)spo2_value);

}


void get_temp(char *temp_value,uint8_t temp,uint8_t humi)
{
//	static uint16_t i =0;
    lv_label_set_text(label_temp,temp_value);
//	
//	line_points[i].x=i*3;
//	line_points[i].y=(uint16_t)temp;
//	
//	line_points_humi[i].x=i*3;
//	line_points_humi[i].y=(uint16_t)(humi/2);
//	

//	if(i>=150)
//	{
//		i=0;
//	}
//	
//	lv_line_set_points(line_temp,line_points,i);
//	lv_line_set_points(line_humi,line_points_humi,i);
	
	set_chart((uint16_t)temp,(uint16_t)humi);
	lv_label_set_text_fmt(temp_label,"temp:%d",temp);
	lv_label_set_text_fmt(humi_label,"humi:%d",humi);
//	
//	i++;
}
//get time value
void lv_get_time(char *rtc_time)
{
	lv_label_set_text(label_left,rtc_time);
}




void download_page_func(void)
{
	
	lv_obj_t *downpage_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(downpage_obj,-10,0);
    lv_obj_set_size(downpage_obj,60,30);
    lv_obj_set_align(downpage_obj,LV_ALIGN_TOP_MID);
    lv_obj_set_style_opa(downpage_obj,0,LV_STATE_DEFAULT);
	
	down_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(down_page,240,320);
    lv_obj_set_x(down_page,0);
    lv_obj_set_y(down_page,-320);
    lv_obj_set_align(down_page,LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(down_page,lv_color_hex(0x141414),LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_clear_flag(down_page,LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t *up_page =lv_obj_create(down_page);
    lv_obj_set_size(up_page,240,40);
    lv_obj_set_align(up_page,LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_bg_color(up_page,lv_color_hex(0x141414),LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(up_page,LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(up_page,lv_color_hex(0x141414),LV_STATE_DEFAULT);


    lv_obj_t *label_dp = lv_label_create(down_page);
    lv_obj_set_align(label_dp,LV_ALIGN_CENTER);
    lv_label_set_text(label_dp,"This is Down Page");
    lv_obj_set_style_text_color(label_dp,lv_color_hex(0x66ccff),LV_STATE_DEFAULT);


    lv_obj_add_event_cb(downpage_obj,event_downpage,LV_EVENT_ALL,NULL);
//    lv_obj_add_event_cb(top_obj,event_downpage,LV_EVENT_ALL,NULL);
    lv_obj_add_event_cb(up_page,event_uppage,LV_EVENT_ALL,NULL);
	
	lv_obj_t *slider_light = lv_slider_create(down_page);
    lv_obj_align(slider_light,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(slider_light,160,50);
    lv_obj_set_style_radius(slider_light,10,LV_PART_MAIN);
    lv_obj_set_style_radius(slider_light,8,LV_PART_INDICATOR);
    lv_obj_remove_style(slider_light,NULL,LV_PART_KNOB);

    lv_obj_set_style_bg_color(slider_light,lv_color_hex(0xECECF0),LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_light,lv_color_hex(0x0078f0),LV_PART_INDICATOR);

    lv_obj_add_event_cb(slider_light,downpage_btn_event,LV_EVENT_VALUE_CHANGED,NULL);

    //down page control btn
    lv_obj_t *downpage_control = lv_obj_create(down_page);
    lv_obj_set_size(downpage_control,210,70);
    lv_obj_align(downpage_control,LV_ALIGN_CENTER,0,-80);
    lv_obj_remove_style(downpage_control,NULL,LV_PART_SCROLLBAR);

    //btns from control
    //wifi
    wifi_btn = lv_btn_create(downpage_control);
    lv_obj_set_size(wifi_btn,50,50);
    lv_obj_align(wifi_btn,LV_ALIGN_LEFT_MID,5,0);
    lv_obj_set_style_radius(wifi_btn,30,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(wifi_btn,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(wifi_btn,downpage_control_event,LV_EVENT_ALL,NULL);


    lv_obj_t *wifi_btn_label = lv_label_create(wifi_btn);
    lv_label_set_text(wifi_btn_label,LV_SYMBOL_WIFI);
    lv_obj_set_size(wifi_btn_label,40,40);
    lv_obj_set_style_text_font(wifi_btn_label,&lv_font_montserrat_24,LV_STATE_DEFAULT);
    lv_obj_align(wifi_btn_label,LV_ALIGN_CENTER,5,10);

//bluetooth btn
    blue_btn = lv_btn_create(downpage_control);
    lv_obj_set_size(blue_btn,50,50);
    lv_obj_set_align(blue_btn,LV_ALIGN_CENTER);
    lv_obj_set_style_radius(blue_btn,30,LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(blue_btn,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(blue_btn,downpage_control_event,LV_EVENT_ALL,NULL);

//blue label
    lv_obj_t *blue_btn_label = lv_label_create(blue_btn);
    lv_label_set_text(blue_btn_label,LV_SYMBOL_BLUETOOTH);
    lv_obj_set_size(blue_btn_label,40,40);
    lv_obj_set_style_text_font(blue_btn_label,&lv_font_montserrat_24,LV_STATE_DEFAULT);
    lv_obj_align(blue_btn_label,LV_ALIGN_CENTER,10,10);

//other btn
    other_btn = lv_btn_create(downpage_control);
    lv_obj_set_size(other_btn,50,50);
    lv_obj_align(other_btn,LV_ALIGN_RIGHT_MID,-5,0);
    lv_obj_set_style_radius(other_btn,30,LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(other_btn,lv_color_hex(0xa8aca8),LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(other_btn,lv_color_hex(0x0078f0),LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(other_btn,downpage_control_event,LV_EVENT_ALL,NULL);

    lv_obj_t *other_btn_label = lv_label_create(other_btn);
    lv_label_set_text(other_btn_label,LV_SYMBOL_DOWNLOAD);
    lv_obj_set_size(other_btn_label,40,40);
    lv_obj_set_style_text_font(other_btn_label,&lv_font_montserrat_24,LV_STATE_DEFAULT);
    lv_obj_align(other_btn_label,LV_ALIGN_CENTER,10,10);



    //音量滑块（下拉页）
    label_down_sl = lv_label_create(slider_light);
    lv_label_set_text(label_down_sl,LV_SYMBOL_VOLUME_MID);
    lv_obj_set_style_text_font(label_down_sl,&lv_font_montserrat_30,0);
    lv_obj_set_style_text_color(label_down_sl,lv_color_hex(0x3a404d),0);
    lv_obj_align(label_down_sl,LV_ALIGN_LEFT_MID,10,0);
	


    //音量滑块标签
	label_down_vol= lv_label_create(down_page);
    lv_label_set_text(label_down_vol,"");
    lv_obj_align_to(label_down_vol,slider_light,LV_ALIGN_OUT_BOTTOM_MID,-30,20);
	
}






//win_btn_event

void win_btn_event(lv_event_t *ev)
{
    lv_obj_t *target = lv_event_get_target(ev);
    lv_event_code_t code =lv_event_get_code(ev);

    if(target == win_wifi_btn)
    {
        lv_obj_add_flag(wifi_win,LV_OBJ_FLAG_HIDDEN);
        printf("wifi win is close\n");
    }
    if(target == win_blue_btn)
    {
        lv_obj_add_flag(blue_win,LV_OBJ_FLAG_HIDDEN);
        printf("wifi win is close\n");
    }
    if(target == win_other_btn)
    {
        lv_obj_add_flag(other_win,LV_OBJ_FLAG_HIDDEN);
        printf("wifi win is close\n");
    }



//lv_disp_load_scr()






}

//wifi win func
void pagedown_wifi_win_func(void)
{
    //wifi win
    wifi_win = lv_win_create(down_page,20);
    lv_obj_set_align(wifi_win,LV_ALIGN_CENTER);
    lv_obj_set_size(wifi_win,170,200);
    lv_obj_t *title = lv_win_add_title(wifi_win, "Setting");

    win_wifi_btn = lv_win_add_btn(wifi_win, LV_SYMBOL_CLOSE, 20);

    lv_obj_set_style_text_color(win_wifi_btn,lv_color_hex(0xff0000),LV_STATE_PRESSED);
    lv_obj_add_event_cb(win_wifi_btn,win_btn_event,LV_EVENT_CLICKED,NULL);


    lv_obj_t *content = lv_win_get_content(wifi_win); 				/* 获取主体 */
    win_wifi_label = lv_label_create(content);
    lv_label_set_text(win_wifi_label," ");
}

//blue win func
void pagedown_blue_win_func(void)
{
    blue_win = lv_win_create(down_page,20);
    lv_obj_set_align(blue_win,LV_ALIGN_CENTER);
    lv_obj_set_size(blue_win,170,200);
    lv_obj_t *title = lv_win_add_title(blue_win, "Setting");

    win_blue_btn = lv_win_add_btn(blue_win, LV_SYMBOL_CLOSE, 20);

    lv_obj_set_style_text_color(win_blue_btn,lv_color_hex(0xff0000),LV_STATE_PRESSED);
    lv_obj_add_event_cb(win_blue_btn,win_btn_event,LV_EVENT_CLICKED,NULL);
	
	lv_obj_t *content = lv_win_get_content(blue_win); 				/* 获取主体 */
    win_blue_label = lv_label_create(content);
    lv_label_set_text(win_blue_label," ");
	lv_obj_add_event_cb(content,usart_blue_event,LV_EVENT_ALL,NULL);
}

//other win func
void pagedown_other_win_func(void)
{
    other_win = lv_win_create(down_page,20);
    lv_obj_set_align(other_win,LV_ALIGN_CENTER);
    lv_obj_set_size(other_win,170,200);
    lv_obj_t *title = lv_win_add_title(other_win, "Setting");

    win_other_btn = lv_win_add_btn(other_win, LV_SYMBOL_CLOSE, 20);

    lv_obj_set_style_text_color(win_other_btn,lv_color_hex(0xff0000),LV_STATE_PRESSED);
    lv_obj_add_event_cb(win_other_btn,win_btn_event,LV_EVENT_CLICKED,NULL);
	
	lv_obj_t *content = lv_win_get_content(other_win); 
	win_other_label = lv_label_create(content);
	lv_label_set_text(win_other_label," ");
	lv_obj_add_event_cb(content,usart_blue_event,LV_EVENT_ALL,NULL);
}







void set_chart(uint16_t y,uint16_t humi_y)
{

        lv_chart_set_next_value(temp_chart,temp_chart_line,y);
		lv_chart_set_next_value(temp_chart,humi_chart_line,humi_y);

}









void my_gui(void)
{

    lv_obj_t *tile_view = lv_tileview_create(lv_scr_act());

    lv_obj_t *tile1 = lv_tileview_add_tile(tile_view,1,1,LV_DIR_RIGHT |LV_DIR_TOP | LV_DIR_LEFT);
    lv_obj_t *tile2 = lv_tileview_add_tile(tile_view,1,0,LV_DIR_BOTTOM );
    lv_obj_t *tile3 = lv_tileview_add_tile(tile_view,0,1,LV_DIR_RIGHT);
    tile4 = lv_tileview_add_tile(tile_view,2,1,LV_DIR_LEFT );

    lv_obj_update_layout(tile_view);
    lv_obj_set_tile(tile_view,tile1,LV_ANIM_OFF);


    lv_obj_t *label_1 = lv_label_create(tile1);
    lv_obj_set_align(label_1,LV_ALIGN_CENTER);
    lv_label_set_text(label_1,"Admin Page");

    lv_obj_t *label_2 = lv_label_create(tile2);
    lv_obj_align(label_2,LV_ALIGN_TOP_MID,0,30);
    lv_label_set_text(label_2,"top page");


    lv_obj_t *label_3 = lv_label_create(tile3);
    lv_obj_set_align(label_3,LV_ALIGN_CENTER);
    lv_label_set_text(label_3,"left page");

    lv_obj_t *label_4 = lv_label_create(tile4);
    lv_obj_set_align(label_4,LV_ALIGN_CENTER);
    lv_label_set_text(label_4,"right page");


    lv_obj_remove_style(tile_view,NULL,LV_PART_SCROLLBAR);


    //顶部标签
    label_left = lv_label_create(lv_scr_act());
    lv_obj_align(label_left,LV_ALIGN_TOP_LEFT,10,10);
    lv_label_set_text(label_left,"");



    lv_obj_t *label_right = lv_label_create(lv_scr_act());
    lv_obj_align(label_right,LV_ALIGN_TOP_RIGHT,-10,10);
    lv_label_set_text(label_right,LV_SYMBOL_WIFI"  100% "LV_SYMBOL_BATTERY_FULL);


    lv_obj_t *slider_light = lv_slider_create(tile2);
    lv_obj_align(slider_light,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(slider_light,50,160);
    lv_obj_set_style_radius(slider_light,10,LV_PART_MAIN);
    lv_obj_set_style_radius(slider_light,8,LV_PART_INDICATOR);
    lv_obj_remove_style(slider_light,NULL,LV_PART_KNOB);

    lv_obj_set_style_bg_color(slider_light,lv_color_hex(0x3a404d),LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_light,lv_color_hex(0xffffff),LV_PART_INDICATOR);

    lv_obj_add_event_cb(slider_light,event_cb,LV_EVENT_VALUE_CHANGED,NULL);




    lv_obj_t *slider_voulum = lv_slider_create(tile1);
    lv_obj_align(slider_voulum,LV_ALIGN_CENTER,0,30);
    lv_obj_set_size(slider_voulum,100,30);
    lv_obj_set_style_radius(slider_voulum,15,LV_PART_MAIN);


    //滑块label
    label_sl = lv_label_create(slider_light);
    lv_label_set_text(label_sl,LV_SYMBOL_VOLUME_MID);
    lv_obj_set_style_text_font(label_sl,&lv_font_montserrat_30,0);
    lv_obj_set_style_text_color(label_sl,lv_color_hex(0x000000),0);
    lv_obj_align(label_sl,LV_ALIGN_BOTTOM_MID,0,-10);


    //音量显示
    label_vol_view = lv_label_create(tile2);
    lv_label_set_text(label_vol_view,"");
    lv_obj_align_to(label_vol_view,slider_light,LV_ALIGN_OUT_BOTTOM_MID,-30,20);



    //键盘
    lv_obj_t *obj = lv_obj_create(tile3);
    lv_obj_set_size(obj,220,200);
    lv_obj_set_align(obj,LV_ALIGN_TOP_MID);

    user = lv_textarea_create(obj);
    lv_textarea_set_max_length(user,10);
    lv_obj_set_size(user,120,35);
    lv_obj_align(user,LV_ALIGN_CENTER,0,-40);
    lv_textarea_set_placeholder_text(user, "username");
    lv_textarea_set_one_line(user,true);
    lv_obj_add_event_cb(user,event_ca,LV_EVENT_ALL,NULL);

    passwd = lv_textarea_create(obj);
    lv_textarea_set_max_length(passwd,10);
    lv_obj_set_size(passwd,120,35);
    lv_obj_align(passwd,LV_ALIGN_CENTER,0,0);
    lv_textarea_set_password_mode(passwd, true);
    lv_textarea_set_placeholder_text(passwd, "password");
    lv_textarea_set_accepted_chars(passwd,"0123456789");
    lv_textarea_set_max_length(passwd,8);
    lv_textarea_set_one_line(passwd,true);
    lv_obj_add_event_cb(passwd,event_ca,LV_EVENT_ALL,NULL);

    btn_login = lv_btn_create(obj);
    lv_obj_align(btn_login,LV_ALIGN_CENTER,0,40);
    lv_obj_set_size(btn_login,80,30);
    lv_obj_set_style_bg_color(btn_login,lv_color_hex(0xff0000),LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn_login,event_ca,LV_EVENT_PRESSED,NULL);


    lv_obj_t *label_btn = lv_label_create(btn_login);
    lv_obj_set_align(label_btn,LV_ALIGN_CENTER);
    lv_label_set_text(label_btn,"Login");



    keyboard  = lv_keyboard_create(tile3);
    lv_keyboard_set_popovers(keyboard, true);

    lv_obj_add_event_cb(keyboard,key_event,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_flag(keyboard,LV_OBJ_FLAG_HIDDEN);




    lv_obj_t *label =lv_label_create(obj);
    lv_obj_align(label,LV_ALIGN_CENTER,0,-70);
//    lv_obj_set_style_text_font(label,&lv_font_montserrat_30,LV_STATE_DEFAULT);
    lv_label_set_text(label,"Login Page");

    //temperature and humidity label
    label_temp = lv_label_create(tile1);
    lv_obj_align(label_temp,LV_ALIGN_CENTER,0,-30);
    lv_label_set_text(label_temp,"");

/***

原版温湿度测试


    //liner
    lv_obj_t *bg_line = lv_obj_create(tile4);
    lv_obj_set_size(bg_line,150,100);
    lv_obj_align(bg_line,LV_ALIGN_CENTER,0,-70);
	
	temp_label = lv_label_create(bg_line);
	lv_obj_set_align(temp_label,LV_ALIGN_TOP_MID);
	lv_label_set_text(temp_label,"");
	

    line_temp = lv_line_create(bg_line);
    lv_obj_set_align(line_temp,LV_ALIGN_BOTTOM_LEFT);

    lv_line_set_points(line_temp,line_points,10);
    lv_line_set_y_invert(line_temp,true);
    lv_obj_set_style_line_rounded(line_temp,true,LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(line_temp,lv_color_hex(0xF6AE54),LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(line_temp,3,LV_STATE_DEFAULT);
//    lv_timer_t *timer = lv_timer_create(sin_timer,10,line_temp);

	

	//湿度显示
	lv_obj_t *bg_line_humi = lv_obj_create(tile4);
	lv_obj_set_size(bg_line_humi,150,100);
	lv_obj_align(bg_line_humi,LV_ALIGN_CENTER,0,70);
	
	humi_label = lv_label_create(bg_line_humi);
	lv_obj_set_align(humi_label,LV_ALIGN_TOP_MID);
	lv_label_set_text(humi_label,"");

	
	line_humi = lv_line_create(bg_line_humi);
	lv_obj_set_align(line_humi,LV_ALIGN_BOTTOM_LEFT);
	
	lv_line_set_points(line_humi,line_points,10);
    lv_line_set_y_invert(line_humi,true);
    lv_obj_set_style_line_rounded(line_humi,true,LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(line_humi,lv_color_hex(0x71B043),LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(line_humi,3,LV_STATE_DEFAULT);
	
***/	

#if 1
/***
新版
***/
	temp_chart = lv_chart_create(tile4);
	lv_obj_set_size(temp_chart,150,100);
    lv_obj_align(temp_chart,LV_ALIGN_CENTER,0,-70);
	temp_chart_line = lv_chart_add_series(temp_chart,lv_color_hex(0xff0000),LV_CHART_AXIS_PRIMARY_X);
	   lv_chart_set_range(temp_chart,LV_CHART_AXIS_PRIMARY_Y,0,100);
   lv_chart_set_range(temp_chart,LV_CHART_AXIS_PRIMARY_X,0,300);
   lv_chart_set_type(temp_chart,LV_CHART_TYPE_LINE);
	lv_chart_set_axis_tick(temp_chart,LV_CHART_AXIS_PRIMARY_Y,5,3,4,2,true,40);
	
	humi_chart_line =lv_chart_add_series(temp_chart,lv_color_hex(0x00ff00),LV_CHART_AXIS_PRIMARY_X);
	
	
	heart_chart = lv_chart_create(tile4);
	lv_obj_set_size(heart_chart,150,100);
    lv_obj_align(heart_chart,LV_ALIGN_CENTER,0,70);
	heart_chart_line = lv_chart_add_series(heart_chart,lv_color_hex(0xff0000),LV_CHART_AXIS_PRIMARY_X);
	lv_chart_set_range(heart_chart,LV_CHART_AXIS_PRIMARY_Y,0,200);
	lv_chart_set_range(heart_chart,LV_CHART_AXIS_PRIMARY_X,0,200);
	lv_chart_set_type(heart_chart,LV_CHART_TYPE_LINE);
	lv_chart_set_axis_tick(heart_chart,LV_CHART_AXIS_PRIMARY_Y,5,3,4,2,true,40);
	
	spo2_chart_line =lv_chart_add_series(heart_chart,lv_color_hex(0x00ff00),LV_CHART_AXIS_PRIMARY_X);


#endif
	download_page_func();


}


#endif // 1

