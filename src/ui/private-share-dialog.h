#ifndef SEAFILE_CLIENT_UI_PRIVATE_SHARE_DIALOG_H
#define SEAFILE_CLIENT_UI_PRIVATE_SHARE_DIALOG_H

#include <QUrl>

#include <QAbstractTableModel>
#include <QCompleter>
#include <QDialog>
#include <QHash>
#include <QScopedPointer>
#include <QSet>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QTableView>
#include "ui_private-share-dialog.h"

#include "account.h"
#include "api/contact-share-info.h"
#include "api/requests.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
// only available in qt 5.0+
#define Q_DECL_OVERRIDE
#endif


class QResizeEvent;

class SharedItemsTableView;
class SharedItemsTableModel;

class PrivateShareDialog : public QDialog, public Ui::PrivateShareDialog
{
    Q_OBJECT
public:
    PrivateShareDialog(const Account& account,
                       const QString& repo_id,
                       const QString& repo_name,
                       const QString& path,
                       bool to_group,
                       QWidget* parent);

    bool requestInProgress() const
    {
        return request_in_progress_;
    }
    void showWarning(const QString& msg);

public slots:
    void onUpdateShareItem(int group_id, SharePermission permission);
    void onUpdateShareItem(const QString& email, SharePermission permission);
    void onRemoveShareItem(int group_id, SharePermission permission);
    void onRemoveShareItem(const QString& email, SharePermission permission);
    void onCompleterActivatedOrHighlighted(const QString& text);
    void onUserNameEditingFinished();

private slots:
    void selectFirstRow();
    void onNameInputEdited();
    void onShareSuccess();
    void onShareFailed(const ApiError& error);
    void onUpdateShareSuccess();
    void onUpdateShareFailed(const ApiError& error);
    void onRemoveShareSuccess();
    void onRemoveShareFailed(const ApiError& error);
    void onFetchGroupsSuccess(const QList<SeafileGroup>& groups);
    void onFetchContactsFailed(const ApiError& error);
    void onOkBtnClicked();
    void onGetSharedItemsSuccess(const QList<GroupShareInfo>& group_shares,
                                 const QList<UserShareInfo>& user_shares);
    void onGetSharedItemsFailed(const ApiError& error);
    void onUserNameChanged(const QString& email);
    void onSearchUsersSuccess(const QList<SeafileUser>& contacts);
    void onSearchUsersFailed(const ApiError& error);

private:
    void createTable();
    void fetchGroupsForCompletion();
    void getExistingShardItems();
    bool validateInputs();
    void toggleInputs(bool enabled);
    void enableInputs();
    void disableInputs();
    SharePermission currentPermission();
    SeafileGroup findGroup(const QString& name);
    QLineEdit* lineEdit() const
    {
        return to_group_ ? groupname_input_->lineEdit() : username_input_;
    }
    void updateUsersCompletion(const QList<SeafileUser>& users);

    Account account_;
    QString repo_id_;
    QString repo_name_;
    QString path_;
    bool to_group_;

    QHash<int, SeafileGroup> groups_;
    // A (email, SeafileUser) map.
    QHash<QString, SeafileUser> users_;

    // A (pattern, possible users for completion) multi map.
    QHash<QString, QSet<SeafileUser> > cached_users_;
    QSet<QString> in_progress_search_requests_;

    SharedItemsTableView* table_;
    SharedItemsTableModel* model_;

    QScopedPointer<PrivateShareRequest> request_;
    QScopedPointer<FetchGroupsRequest> fetch_groups_request_;
    QScopedPointer<GetPrivateShareItemsRequest> get_shared_items_request_;
    QScopedPointer<QCompleter> completer_;

    bool request_in_progress_;

    QLineEdit* username_input_;
    QComboBox* groupname_input_;
};

class SharedItemsHeadView : public QHeaderView
{
    Q_OBJECT
public:
    SharedItemsHeadView(QWidget* parent = 0);

    QSize sectionSizeFromContents(int index) const Q_DECL_OVERRIDE;
};

class SharedItemsTableView : public QTableView
{
    Q_OBJECT
public:
    SharedItemsTableView(QWidget* parent = 0);

    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    void setModel(QAbstractItemModel* model) Q_DECL_OVERRIDE;

    SharedItemsTableModel* sourceModel()
    {
        return source_model_;
    }

private:
    SharedItemsTableModel* source_model_;
};

class SharedItemsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SharedItemsTableModel(ShareType share_type, QObject* parent = 0);

    void setShareInfo(const QList<GroupShareInfo>& group_shares,
                      const QList<UserShareInfo>& user_shares);

    void addNewShareInfo(UserShareInfo info);
    void addNewShareInfo(GroupShareInfo info);

    bool shareExists(int group_id);
    bool shareExists(const QString& email);

    GroupShareInfo shareInfo(int group_id);
    UserShareInfo shareInfo(const QString& email);

    void shareOperationSuccess();
    void shareOperationFailed(PrivateShareRequest::ShareOperation op);

    int rowCount(const QModelIndex& parent = QModelIndex()) const
        Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const
        Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role) Q_DECL_OVERRIDE;
    bool removeRows(int row,
                    int count,
                    const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;

    int nameColumnWidth() const
    {
        return name_column_width_;
    }
signals:
    void updateShareItem(int group_id, SharePermission permission);
    void updateShareItem(const QString& email, SharePermission permission);
    void removeShareItem(int group_id, SharePermission permission);
    void removeShareItem(const QString& email, SharePermission permission);

public slots:
    void onResize(const QSize& size);

private:
    bool isGroupShare() const;

    QList<UserShareInfo> user_shares_, previous_user_shares_;
    QList<GroupShareInfo> group_shares_, previous_group_shares_;

    ShareType share_type_;
    int name_column_width_;

    GroupShareInfo removed_group_share_;
    UserShareInfo removed_user_share_;
};


class SharedItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SharedItemDelegate(QObject* parent = 0);

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const Q_DECL_OVERRIDE;

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const Q_DECL_OVERRIDE;
};


#endif // SEAFILE_CLIENT_UI_PRIVATE_SHARE_DIALOG_H
