// MSTFactory.hpp
#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

#include "MSTAlgo.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include <memory>
#include "ThreadPool.hpp"

class MSTFactory {
public:
    static std::unique_ptr<MSTAlgo> create(Command commandType) {
        switch (commandType) {
            case Command::Prim:
                return std::make_unique<PrimMST>();
            case Command::Kruskal:
                return std::make_unique<KruskalMST>();
            default:
                return nullptr;
        }
    }
};

#endif
