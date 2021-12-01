#include "AddRemoveText.h"

AddRemoveText::AddRemoveText() {
    //
}

void AddRemoveText::addText() {
    //
}

void AddRemoveText::removeText() {
    //
}

AddText::AddText() :
    AddRemoveText()
{
    //
}

void AddText::undo() {
    removeText();
}

void AddText::redo() {
    addText();
}

RemoveText::RemoveText() :
    AddRemoveText()
{
    //
}

void RemoveText::undo() {
    addText();
}

void RemoveText::redo() {
    removeText();
}
