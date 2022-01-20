#pragma once

#include "ECSTypes.h"

namespace ecs{

class View
{
public:


    View()
    {}

    struct Iterator
    {
        Iterator() {}

        EntityId operator*() const
        {
            // give back the entityID we're currently at
        }

        bool operator==(const Iterator& other) const
        {
            // Compare two iterators
        }

        bool operator!=(const Iterator& other) const
        {
            // Similar to above
        }

        Iterator& operator++()
        {
            // Move the iterator forward
        }
    };

    const Iterator begin() const
    {
        // Give an iterator to the beginning of this view
    }

    const Iterator end() const
    {
        // Give an iterator to the end of this view 
    }



};


}

