#include <AMD/tree.hpp>

namespace AMD { namespace detail {

Tree::Tree (const std::string& info, 
            const boost::shared_ptr<Tree>& left, 
            const boost::shared_ptr<Tree>& right)
{

}
                   
Tree::~Tree()
{

}

bool Tree::operator=(const Tree& other) const
{
    return false;
}

} } // namespace AMD::detail
