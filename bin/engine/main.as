void init()
{
}

void update()
{
    Event ev;
    while (!events.lastevent()) {
        ev = events.poll();

        print("event " + ev.event + "(" + ev.data  + ")");
    }
}

void draw()
{
    ui.print(10, 10, "FPS: " + engine.getFPS());
    ui.print("fonts/Hack-Regular.ttf:14", 20, 10, "FPS: " + engine.getFPS());
}
