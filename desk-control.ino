#include "wifi_info.h"
#include "log.h"
#include "desk.h"
#include "homekit.h"
#include "buttons.h"

int UP_BUTTON = D3;
int DOWN_BUTTON = D4;
int UP_RELAY = D1;
int DOWN_RELAY = D2;

int TRIG_PIN = D6;
int ECHO_PIN = D7;
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);

Desk desk = Desk(UP_RELAY, DOWN_RELAY, sr04);

ButtonWatcher buttons = ButtonWatcher(UP_BUTTON, DOWN_BUTTON);

void change_state(bool up)
{
	const bool already_moving = desk.isMoving();
	if (!already_moving)
	{
		desk.trigger_move(up);
	}
	else
	{
		desk.stop();
	}
}
void s_stand()
{
	change_state(true);
	server.send(200, "text/html", updateWebpage(true));
}
void s_sit()
{
	change_state(false);
	server.send(200, "text/html", updateWebpage(false));
}
void s_set()
{
	desk.set_stand_height();
	server.send(200, "text/html", updateWebpage(cha_desk_up.value.bool_value));
}

void cha_desk_up_setter(const homekit_value_t value)
{
	bool up = value.bool_value;
	change_state(up);
};

void setup()
{
	Serial.begin(9600);
	wifi_connect();
	cha_desk_up.setter = cha_desk_up_setter;
	homekit_setup();

	setup_server(s_stand, s_sit, s_set);
}

static uint32_t next_check_time = 0;
void loop()
{
	server.handleClient();
	const uint32_t now = millis();
	const bool run_checks = now > next_check_time;
	if (run_checks)
	{
		next_check_time = now + 10 * 1000;
	}
	homekit_loop(run_checks);
	const ButtonState button_state = buttons.get_state(now);
	switch (button_state)
	{
	case UP_AUTO:
		desk.trigger_move(true);
		break;
	case DOWN_AUTO:
		desk.trigger_move(false);
		break;
	case UP_MAN:
		desk.go_up();
		break;
	case DOWN_MAN:
		desk.go_down();
		break;
	case STOP_MAN:
		desk.stop();
		break;
	case DOUBLE_HOLD:
		desk.set_stand_height();
		break;
	case NO_STATE:
		desk.loop(run_checks);
		break;
	}
}