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

void swap(Tree& other)
{

}

std::string Tree::info() const
{
    return this->d_info;
}

void Tree::setInfo(const std::string& info)
{
    this->d_info = info;
}

bool Tree::operator==(const Tree& other) const
{
    return false;
}

bool Tree::operator!=(const Tree& other) const
{
    return true;
}

} } // namespace AMD::detail
