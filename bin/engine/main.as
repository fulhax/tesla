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

void drawEntities()
{
    int count = engine.getEntitiesCount();

    for(int i=0; i < count; i++) {
        Entity@ e = engine.getEntityById(i);
        if(e !is null) {
            e.draw();
        }
    }
}

void drawUi()
{
    ui.print("fonts/Hack-Regular.ttf:32", 10, 10, "FPS: " + engine.getFPS());
    ui.print("fonts/Hack-Regular.ttf:32", 10, 52, "ms: " + engine.getMS());
}

void draw()
{
    drawEntities();
    drawUi();
}
