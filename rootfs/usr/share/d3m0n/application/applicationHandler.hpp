
class applicationHandler {
    public:
    void loadApp(char* appPath);
    void runApp(Application app);
}

struct Application
{
    char* name;
    char* package;
    char* icon;
    char* perms;
    char* start_path;
    char* category;
};
