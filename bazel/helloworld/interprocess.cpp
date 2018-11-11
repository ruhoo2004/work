#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib>
#include <sstream>

int main(int argc, char* argv[])
{
    using namespace boost::interprocess;
    using MyType = std::pair<double, int>;

    if (argc == 1) {
        struct shm_remote
        {
            shm_remote() { shared_memory_object::remove("MySharedM"); }
            ~shm_remote() { shared_memory_object::remove("MySharedM"); }
        } remover;

        managed_shared_memory segment(create_only, "MySharedM", 65536);
        segment.construct<MyType>
            ("MyType instance")
            (0.0, 0);
        segment.construct<MyType>
            ("MyType array")
            [10]
            (0.0, 0);

        float float_initializer[3] = {0.0, 1.0, 2.0};
        int int_initializer[3] = {0, 1, 2};
        segment.construct_it<MyType>
            ("MyType array from it")
            [3]
            (&float_initializer[0], &int_initializer[0]);

        std::string s(argv[0]); s+= " child";
        if (0 != std::system(s.c_str()))
            return 1;

        if (segment.find<MyType>("MyType array").first ||
            segment.find<MyType>("MyType instance").first ||
            segment.find<MyType>("MyType array from it").first)
            return 1;
    }
    else {
        managed_shared_memory segment(open_only, "MySharedM");
		std::pair<MyType*, managed_shared_memory::size_type> res;

		//Find the array
		res = segment.find<MyType> ("MyType array");
		//Length should be 10
		if(res.second != 10) return 1;

		//Find the object
		res = segment.find<MyType> ("MyType instance");
		//Length should be 1
		if(res.second != 1) return 1;

		//Find the array constructed from iterators
		res = segment.find<MyType> ("MyType array from it");
		//Length should be 3
		if(res.second != 3) return 1;

		//We're done, delete all the objects
		segment.destroy<MyType>("MyType array");
		segment.destroy<MyType>("MyType instance");
		segment.destroy<MyType>("MyType array from it");
    }
    return 0; 
}
