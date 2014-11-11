///
/// General QListWidgetItem that can contain a pointer to an
/// object of class T.
///
/// @author Emanuel Laude and Zorah Lähner
///
///

#ifndef ShapeAnalyzer_qtListWidgetItem_h
#define ShapeAnalyzer_qtListWidgetItem_h

#include <QString>
#include <QListWidgetItem>

template<class T>
class qtListWidgetItem : public QListWidgetItem {
public:
    /// \brief Constructor.
    /// \details Creates a new QListWidgetItem with a pointer to item.
    /// @param text name that should be displayed in the QListWidget
    /// @param item pointer to the item this list item is referring to.
    /// @param view optional pointer to the ListWidget
    /// @param type see reference of QListWidgetItem for details
    qtListWidgetItem(
                  const QString &text,
                  T*            item,
                  QListWidget   *view   = 0,
                  int           type    = Type
                  ) : QListWidgetItem(text, view, type), item_(item) {};
    
    /// Returns a pointer to the referenced item.
    T* getItem() {
        return item_;
    }
private:
    T* item_;
};

#endif
