#ifndef IRSPCOMMAND_H
#define IRSPCOMMAND_H

namespace CCPlayer
{

class ICommandResponse
{
public:
    virtual void OnCommandOpen(int ErrCode) = 0;
    virtual void OnCommandStop(int ErrCode) = 0;
};

}
#endif
