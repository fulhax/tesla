void init()
{
}

void update()
{
    while(events.count() != 0) {
        string ev = events.poll();
        print("event " + ev);
    }
}

void draw()
{
    ui.print(10, 10, "FPS: " + engine.getFPS());
    ui.print("fonts/Hack-Regular.ttf:14", 20, 10, "FPS: " + engine.getFPS());
}
