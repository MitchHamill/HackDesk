#include <arduino_homekit_server.h>
#include "SR04.h"

#include "log.h"

extern "C" homekit_characteristic_t cha_desk_up;

int DEF_TRIG_PIN = D3;
int DEF_ECHO_PIN = D2;

class Desk
{
private:
    SR04 sr04 = SR04(0, 0);
    bool moving_from_req;
    uint32_t req_time;
    bool standing;
    bool up;

    int UP_RELAY_PIN;
    int DOWN_RELAY_PIN;

    void check_height()
    {
        // LOG_D("Checking Desk State against memory. Standing: %d, Up: %d", standing, up)
        if (standing && !up)
        {
            // LOG_D("Detected standing w/o change from client.");
            cha_desk_up.value.bool_value = true;
            homekit_characteristic_notify(&cha_desk_up, cha_desk_up.value);
        }
        else if (!standing && up)
        {
            // LOG_D("Detected sitting w/o change from client.");
            cha_desk_up.value.bool_value = false;
            homekit_characteristic_notify(&cha_desk_up, cha_desk_up.value);
        }
    }
    void check_for_stop()
    {
        const uint32_t t = millis();
        // LOG_D("Clock: %d, Req Time: %d, Diff: %f", t, req_time, (float)(t - req_time) / 1000)
        if (up && standing)
        {
            stop();
            moving_from_req = false;
        }
        if (!up && (t - req_time) / 1000 > 30)
        {
            stop();
            moving_from_req = false;
            req_time = 0;
        }
    }

    void refresh_statuses()
    {
        int STAND_HEIGHT = 112;
        long dist = sr04.DistanceAvg(10, 10);
        standing = dist > STAND_HEIGHT;

        up = cha_desk_up.value.bool_value;
    }

public:
    Desk(int UP_RELAY, int DOWN_RELAY, SR04 IN_SR04);
    bool isMoving() { return moving_from_req; };
    void stop()
    {
        digitalWrite(UP_RELAY_PIN, LOW);
        digitalWrite(DOWN_RELAY_PIN, LOW);

        refresh_statuses();
        check_height();
    }
    void go_up()
    {
        digitalWrite(UP_RELAY_PIN, HIGH);
        digitalWrite(DOWN_RELAY_PIN, LOW);
    }
    void go_down()
    {
        digitalWrite(DOWN_RELAY_PIN, HIGH);
        digitalWrite(UP_RELAY_PIN, LOW);
    }

    void loop(bool should_check)
    {
        if (moving_from_req)
        {
            refresh_statuses();
            check_for_stop();
        }
        else if (should_check)
        {
            refresh_statuses();
            check_height();
        }
    }

    void trigger_move(bool stand)
    {
        moving_from_req = true;
        cha_desk_up.value.bool_value = stand;

        refresh_statuses();
        if (stand && !standing)
        {
            go_up();
        }
        if (!stand)
        {
            req_time = millis();
            go_down();
        }
    }

    void set_stand_height()
    {
        int height_now = sr04.DistanceAvg(10, 50);
        // LOG_D("Current Height: %d", height_now);
    }
};

Desk::Desk(int UP_RELAY, int DOWN_RELAY, SR04 IN_SR04)
{
    UP_RELAY_PIN = UP_RELAY;
    DOWN_RELAY_PIN = DOWN_RELAY;
    sr04 = IN_SR04;
    pinMode(UP_RELAY_PIN, OUTPUT);
    pinMode(DOWN_RELAY_PIN, OUTPUT);
}
