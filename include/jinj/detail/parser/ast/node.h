typedef struct _JinjParserASTNode {
    enum _JinjParserASTNodeType {
        _Dummy,
        // TODO
    } type;
    union _JinjParserASTNodeData {

    } value;
} _JinjParserASTNode;
