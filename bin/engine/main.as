void init()
{
}

void update()
{
    while(events.count() != 0) {
        Event ev = events.poll();
        print("event " + ev.event + "(" + ev.data  + ")");
    }
}

void draw()
{
    ui.print(10, 10, "FPS: " + engine.getFPS());
    ui.print("fonts/Hack-Regular.ttf:14", 20, 10, "FPS: " + engine.getFPS());
}
