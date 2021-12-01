#ifndef ADDREMOVETEXT_H
#define ADDREMOVETEXT_H

#include <QUndoCommand>

class AddRemoveText : public QUndoCommand {
public:
    AddRemoveText();
    
protected:
    void addText();
    void removeText();
};

class AddText : public AddRemoveText {
public:
    AddText();
    
    void undo();
    void redo();
};

class RemoveText : public AddRemoveText {
public:
    RemoveText();
    
    void undo();
    void redo();
};

#endif // ADDREMOVETEXT_H
