#pragma once

#include <QAbstractItemModel>
#include <QVariant>
#include <QString>
#include <vector>
#include <memory>

class CodingEngine;
struct CodingFieldDef;

struct TreeNode
{
    QString name;
    QString type;
    int bitOffset = 0;
    int bitWidth = 0;
    int absoluteBitOffset = 0; // resolved offset for nested fields
    QVariantList options;
    QVariantMap dependentOptions;
    QString dependsOn;

    TreeNode *parent = nullptr;
    std::vector<std::unique_ptr<TreeNode>> children;
    int row = 0;

    // Delete copy constructor and assignment operator to prevent copying
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;
    
    // Allow move constructor and assignment
    TreeNode(TreeNode&&) = default;
    TreeNode& operator=(TreeNode&&) = default;
    
    // Default constructor
    TreeNode() = default;
};

class CodingTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        BitOffsetRole,
        BitWidthRole,
        AbsoluteBitOffsetRole,
        ValueRole,
        DisplayValueRole,
        OptionsRole,
        DependentOptionsRole,
        DependsOnRole,
        EditableRole
    };

    explicit CodingTreeModel(CodingEngine *engine, QObject *parent = nullptr);

    // QAbstractItemModel interface
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setFieldValue(int absoluteBitOffset, int bitWidth, int value);
    [[nodiscard]] Q_INVOKABLE QString getDisplayValue(int absoluteBitOffset, int bitWidth, const QVariantList &options) const;

private:
    void addFieldNodes(TreeNode *parent, const std::vector<CodingFieldDef> &fields, int parentAbsOffset);

    CodingEngine *m_engine = nullptr;
    std::unique_ptr<TreeNode> m_root;
};
