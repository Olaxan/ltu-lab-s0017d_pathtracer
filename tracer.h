class Raytracer;
class ThreadHelper
{
public:
    ThreadHelper(Raytracer* owner, void *arg) : owner(owner), arg(arg) {}
 
    void *launch() { return owner->trace_helper(arg); }
 
    Raytracer *owner;
    void *arg;
};
 
void *LaunchMemberFunction(void *obj)
{
    ThreadHelper *helper = reinterpret_cast<ThreadHelper *>(obj);
    return helper->launch();
}
