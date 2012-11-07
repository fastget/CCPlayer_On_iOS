#ifndef IRSPCOMMAND_H
#define IRSPCOMMAND_H

namespace CCPlayer
{

class ICommandResponse
{
public:
    virtual void OnCmmandOpen(int ErrCode) = 0;
};

}
#endif
