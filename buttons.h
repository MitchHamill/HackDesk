#include "log.h"

enum ButtonState
{
    NO_STATE,

    UP_AUTO,
    DOWN_AUTO,

    UP_MAN,
    DOWN_MAN,
    STOP_MAN,

    DOUBLE_HOLD,
};

class ButtonWatcher
{
private:
    int UP_PIN;
    int DOWN_PIN;
    uint32_t up_start = 0;
    uint32_t down_start = 0;
    uint32_t double_start = 0;

    bool up_pressed() { return (digitalRead(UP_PIN) == LOW); };
    bool down_pressed() { return (digitalRead(DOWN_PIN) == LOW); };

    bool manual_released = false;
    bool double_released = false;

public:
    ButtonWatcher(int UP_PIN, int DOWN_PIN);

    ButtonState get_state(int now)
    {
        bool up = up_pressed();
        bool down = down_pressed();
        bool double_pressed = (up && down);
        if (up && !up_start && !double_start)
        {
            up_start = now;
        }
        if (down && !down_start && !double_start)
        {
            down_start = now;
        }
        if (up_start > 0 && down_start > 0 && !double_start)
        {
            double_start = now;
            up_start = 0;
            down_start = 0;
        }
        else if (up && up_start && now - up_start > 500 && !manual_released)
        {
            manual_released = true;
            return UP_MAN;
        }
        else if (down && down_start && now - down_start > 500 && !manual_released)
        {
            manual_released = true;
            return DOWN_MAN;
        }
        else if (double_pressed && double_start && now - double_start > 1500 && !double_released)
        {
            double_released = true;
            return DOUBLE_HOLD;
        }
        else if (!up && up_start && now - up_start <= 500)
        {
            up_start = 0;
            return UP_AUTO;
        }
        else if (!down && down_start && now - down_start <= 500)
        {
            down_start = 0;
            return DOWN_AUTO;
        }
        else if (!double_start && ((!up && up_start && now - up_start > 500) || (!down && down_start && now - down_start > 500)))
        {
            up_start = 0;
            down_start = 0;
            manual_released = false;
            return STOP_MAN;
        }
        else if (double_start && !up && !down)
        {
            double_start = 0;
            double_released = false;
        }

        return NO_STATE;
    }
};

ButtonWatcher::ButtonWatcher(int UP_PIN_ARG, int DOWN_PIN_ARG)
{
    pinMode(UP_PIN, INPUT_PULLUP);
    pinMode(DOWN_PIN, INPUT_PULLUP);
    UP_PIN = UP_PIN_ARG;
    DOWN_PIN = DOWN_PIN_ARG;
}
