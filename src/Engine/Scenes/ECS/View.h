#pragma once

#include "ECSTypes.h"
#include "Engine/Scenes/ECS/ComponentPool.h"

namespace ecs{

class View
{
public:


    View(Signature signature, std::vector<EntityId>* entities, std::array<Signature, MAX_ENTITIES>* entity_signatures, std::vector<std::shared_ptr<ComponentPoolBase>>* pools)
    : m_signature(signature), m_entities(entities), m_entity_signatures(entity_signatures), m_component_pools(pools)
    {

    }   


    //Iterator used when iterating view with range based for loop
    class Iterator
    {
    public:
        Iterator(EntityId entity_index, Signature entity_signature, std::vector<EntityId>* entities,
            std::array<Signature, MAX_ENTITIES>* entity_signatures) :
            entity_index(entity_index), signature(entity_signature), entities(entities), entity_signatures(entity_signatures)
            {}

        EntityId operator*() const
        {
            //give back the entityID we're currently at
            return entity_index;
        }

        bool operator==(const Iterator& other) const
        {
            //Compare two iterators
            return  entity_index == other.entity_index || entity_index == entities->size();
        }

        bool operator!=(const Iterator& other) const
        {
            return  entity_index != other.entity_index && entity_index != entities->size();
        }

        inline bool isSignatureCorrect(EntityId id)
        {
            return
                //check that entity is not -1(valid)
                (isEntityIdValid(entities->at(id))) &&
                //check that entity has matching signature
                (signature == (signature & entity_signatures->at(id)));
        }

        Iterator& operator++()
        {
            //Move the iterator forward
            do
            {
                entity_index++;
            } 
            while (entity_index < entities->size() && !isSignatureCorrect(entity_index));
            return *this;
        }
         

    private:
        EntityId entity_index;
        Signature signature;

        std::vector<EntityId>* entities;
        std::array<Signature, MAX_ENTITIES>* entity_signatures;
    };


    const Iterator begin() const
    {
        //Give iterator to the beginning of this view
        EntityId first_index = 0;

        while ((first_index < m_entities->size()) && (isEntityIdValid(m_entities->at(first_index))) && (m_signature != (m_signature & m_entity_signatures->at(first_index) )))
        {
            first_index++;
        }


        return Iterator(first_index, m_signature, m_entities, m_entity_signatures);
    }


    const Iterator end() const
    {
        //Give iterator to the end of this view 
        return Iterator(m_entities->size(), m_signature, m_entities, m_entity_signatures);
    }


    /*
    *   Gets component from entity given. 
    *   This function asumes that component type used was used in the view. 
    *   Using another component type results in undefined behaviour
    *
    */


    template<typename ComponentType>
    ComponentType& get(EntityId entity)
    {
        ComponentId comp_id = getComponentId<ComponentType>();

        ComponentPool<ComponentType>* pool = static_cast<ComponentPool<ComponentType>*>(m_component_pools->at(comp_id).get());
       
        return pool->getComponent(entity);

    }









private:
    
    Signature m_signature;
    std::vector<EntityId>* m_entities;
    std::array<Signature, MAX_ENTITIES>* m_entity_signatures;
    std::vector<std::shared_ptr<ComponentPoolBase>>* m_component_pools;

};

}

