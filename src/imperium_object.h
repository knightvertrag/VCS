#ifndef IMPERIUM_IMPERIUM_OBJECT_H_
#define IMPERIUM_IMPERIUM_OBJECT_H_

#include "repository.h"

namespace imperium
{
    class Impobject
    {
    public:
        imperium::Repository repo;
        std::string type;
        std::string data;
        //template <typename T>
        Impobject(imperium::Repository repo, std::string data, std::string type) : repo(repo), type(type), data(data){};
        // void serialize();
        // void deserialize();
    };

    /**
     * Read the Impobject_id from Imperium Repo
     * 
     * @param repo Repository whose Impobjects need to be read
     * @param sha SHA-1 to extract path
     * @return Imperium Impobject required
    */
    Impobject object_read(Repository repo, std::string sha);
    template <typename T>
    std::string object_write(T obj, bool actually_write = true);

    class Blobobject : public Impobject
    {
    public:
        Blobobject(imperium::Repository repo, std::string data);
        std::string serialize();
        void deserialize(std::string data);
    };

    class Treeobject : public Impobject
    {
    public:
        Treeobject(Repository repo, std::string data);
        void serialize(Treeobject treeobj);
    };

    class Commitobject : public Impobject
    {
    public:
        Commitobject(Repository repo, std::string data);
        void serialize(Commitobject commitobj);
    };

    class Tagobject : public Impobject
    {
    public:
        Tagobject(Repository repo, std::string data);
        void serialize(Tagobject tagobj);
    };
}

#endif // IMPERIUM_OBJECT_H_