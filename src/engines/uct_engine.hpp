//
// Created by alex on 16-12-21.
//

#ifndef GO_2016_UCT_ENGINE_HPP
#define GO_2016_UCT_ENGINE_HPP

#include <algorithm>
#include <iterator>
#include <string>
#include <tuple>
#include <list>
#include <vector>
#include <cstdlib>
#include <gtplib/gtpengine.hpp>
#include "board.hpp"
#include <numeric>
#include "logger.hpp"
#include "simple_engine_base.hpp"
#include "mct_class.hpp"
#include <chrono>

namespace engines{
    using namespace gtp;
    template<std::size_t BOARDSIZE>
    class UCTEngine : public SimpleEngineBase {
    private:
        board::Board<BOARDSIZE, BOARDSIZE> board;
        bool received_pass = false;
        float komi = 6.5;
        int threadNums = 8;
        int searchTime = 10;

        static board::Player colorToPlayer(Color c)
        {
            switch (c)
            {
                case Color::black:
                    return board::Player::B;
                case Color::white:
                    return board::Player::W;
            }
        }
    public:
        UCTEngine() = default;
        virtual std::string handle(const CmdName& cmd) override
        {
            logger->debug("Received CmdName");
            return "UCTEngine";
        }
        virtual std::string handle(const CmdVersion& cmd) override
        {
            logger->debug("Received CmdVersion");
            return "1.0";
        }
        virtual void handle(const CmdBoardSize& cmd) override
        {
            std::size_t size = std::get<0>(cmd.params);
            logger->debug("Received boardsize: {}", size);
            if (size != decltype(board)::w)
            {
                logger->error("Unsupported size: {}", size);
                throw std::runtime_error("Unsupported size");
            }
        }
        virtual void handle(const CmdClearBoard& cmd) override
        {
            logger->debug("Received clear_board");
            received_pass = false;
            board.clear();
        }
        virtual void handle(const CmdKomi& cmd) override
        {
            komi = std::get<0>(cmd.params);
            logger->debug("Received komi {}", komi);
        }
        virtual VertexOrPass handle(const CmdGenmove& cmd) override
        {
            logger->debug("Received genmove");
            Color c = std::get<0>(cmd.params);

            //using PT = typename decltype(board)::PointType;
            mct::MCT<19,19> tree(board, colorToPlayer(c), threadNums);
            //std::cout << "Tree has built!" << std::endl;
            mct::Action<19,19> nextAction=tree.MCTSearch(searchTime);
            //std::cout << "Search has completed!" << std::endl;
            if(nextAction.pass == true){
            	  //std::cout << "Pass!" << std::endl;
                return Pass();
            }else{
                //logger->trace("Valid pos: ({}, {})  ", (int)nextAction.point.x, (int)nextAction.point.y);
                logger->debug("Choose to place at {}, {}", (int)nextAction.point.x + 1, (int)nextAction.point.y + 1);
                board.place(nextAction.point, colorToPlayer(c));
                logger->debug("After genmove \n {}", board);
                return VertexOrPass(nextAction.point.x + 1, nextAction.point.y + 1);
            }
        }
        virtual void handle(const CmdPlay& cmd) override
        {
            Move move = std::get<0>(cmd.params);
            Color c = move.color;
            if (move.vertex.type == VertexOrPassType::VERTEX)
            {
                using PT = typename decltype(board)::PointType;
                Vertex v = (Vertex)(move.vertex);
                logger->debug("Received Play at ({}, {})", v.x - 1, v.y - 1);
                board.place(PT {(char)(v.x - 1), (char)(v.y - 1)}, colorToPlayer(c));
            }
            else
            {
                logger->debug("Received PASS");
                received_pass = true;
            }
            logger->debug("After play \n {}", board);
        }
        virtual void handle(const CmdTimeSettings& cmd) override
        {
            int t1 = std::get<0>(cmd.params), t2 = std::get<1>(cmd.params), t3 = std::get<2>(cmd.params);
            logger->debug("Received time setting: {}, {}, {}", t1, t2, t3);
        }
        virtual void handle (const CmdTimeLeft& cmd) override
        {
            Color c1 = std::get<0>(cmd.params), c2 = std::get<1>(cmd.params);
            int t1 = std::get<2>(cmd.params), t2 = std::get<3>(cmd.params);
            logger->debug("Received Time left: {} time for {}; {} time for {}", t1, (int)c1, t2, (int)c2);
        }

        virtual ~UCTEngine()
        {
            logger->debug("UCTEngine destroyed!");
        }
    };
}

#endif //GO_2016_UCT_ENGINE_HPP
