#ifndef SERVER_BUILDER
#define SERVER_BUILDER

#include "HttpServer.h"

class ServerBuilder {

    public:

        ServerBuilder(Router& Router, ServerStartDetails& SSD) : 
            MyRouter_{Router},
            SSD_(SSD),
            IOContext_{1},
            Acceptor_{
                IOContext_,
                    {
                        SSD_.Address,
                        SSD_.Port
                    }}
        {};

        void Start(){
            for (int i(0); i < SSD_.NumberOfWorkers; ++i){
                    try
                    {
                        auto Candidate(
                            HttpWorker::Create(
                                Acceptor_,
                                SSD_.DocumentRoot,
                                MyRouter_));
                        if (Candidate)
                        {
                            try
                            {
                                Candidate->Start();
                                Workers.push_back(Candidate);
                            }
                            catch (const std::exception &)
                            {
                                std::cerr << "Error Starting worker" << std::endl;
                            }
                        }
                    }
                    catch (const std::bad_alloc &)
                    {
                        std::cerr << "Out of memory for workers" << std::endl;
                        break;
                    }
                }

                if (SSD_.Spin)
                {
                    do
                    {
                        IOContext_.poll();
                    } while (true);
                }
                else
                {
                    IOContext_.run();
                }
        }


    private:
        Router& MyRouter_;
        ServerStartDetails& SSD_;
        boost::asio::io_context IOContext_;
        boost::asio::ip::tcp::acceptor Acceptor_;
        std::list<HttpWorkerPtr> Workers;
};





#endif