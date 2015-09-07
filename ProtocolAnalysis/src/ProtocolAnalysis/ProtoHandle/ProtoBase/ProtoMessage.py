#-*- encoding=utf-8 -*-


from ProtocolAnalysis.ProtoHandle.ProtoBase.ProtoElemBase import ProtoElemBase, eProtoElemType
from ProtocolAnalysis.ProtoHandle.ProtoBase.ProtoKeyWord import ProtoKeyWord
from ProtocolAnalysis.ProtoHandle.ProtoBase.MessageMember import MessageMember


class ProtoMessage(ProtoElemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        super(ProtoMessage, self).__init__(eProtoElemType.eMessage)


    def parse(self, tokenParseBuffer):
        self.m_typeKeyWord = tokenParseBuffer.getTokenAndRemove()  # "message"
        self.m_typeName = tokenParseBuffer.getTokenAndRemove() # "stTest"
        tokenParseBuffer.getTokenAndRemove()        # 移除 "{"
        
        while True:
            linePrefix = tokenParseBuffer.getTokenAndNoRemove()
            if linePrefix == ProtoKeyWord.eRightBraceSemicolon:     # 如果取出来是 "};"
                break
            elif linePrefix == ProtoKeyWord.eRightBrace:            # 如果取出来是 "}"
                tokenParseBuffer.getTokenAndRemove()        # 移除 ";"
                break 
            else:
                messageMember = MessageMember()
                self.m_memberList.append(messageMember)
                messageMember.parse(tokenParseBuffer)
            
        
        tokenParseBuffer.getTokenAndRemove()        # 移除 "};"
        
