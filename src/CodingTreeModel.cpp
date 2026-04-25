#include "CodingTreeModel.h"
#include "CodingEngine.h"

CodingTreeModel::CodingTreeModel(CodingEngine *engine, QObject *parent)
    : QAbstractItemModel(parent)
    , m_engine(engine)
{
    m_root = std::make_unique<TreeNode>();
    m_root->name = "root";

    connect(m_engine, &CodingEngine::configLoaded, this, &CodingTreeModel::refresh);
    connect(m_engine, &CodingEngine::hexStringChanged, this, [this]() {
        if (m_root && !m_root->children.empty()) {
            // Notify all visible rows (including nested) that values changed
            emit dataChanged(index(0, 0), index(rowCount() - 1, 0), {ValueRole, DisplayValueRole});

            // Also notify children of expanded nodes
            for (int i = 0; i < rowCount(); ++i) {
                QModelIndex parent = index(i, 0);
                if (rowCount(parent) > 0) {
                    emit dataChanged(index(0, 0, parent), index(rowCount(parent) - 1, 0, parent), {ValueRole, DisplayValueRole});
                }
            }
        }
    });
}

QModelIndex CodingTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)){
        return {};
    }

    TreeNode *parentNode = parent.isValid() ? static_cast<TreeNode *>(parent.internalPointer()) : m_root.get();

    if (row < 0 || row >= static_cast<int>(parentNode->children.size())) {
        return {};
    }

    return createIndex(row, column, parentNode->children[row].get());
}

QModelIndex CodingTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }

    auto *childNode = static_cast<TreeNode *>(child.internalPointer());
    TreeNode *parentNode = childNode->parent;

    if (!parentNode || parentNode == m_root.get()) {
        return {};
    }

    return createIndex(parentNode->row, 0, parentNode);
}

int CodingTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeNode *node = parent.isValid()
        ? static_cast<TreeNode *>(parent.internalPointer())
        : m_root.get();

    return static_cast<int>(node->children.size());
}

int CodingTreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant CodingTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    auto *node = static_cast<TreeNode *>(index.internalPointer());

    switch (role) {
    case NameRole:
        return node->name;
    case TypeRole:
        return node->type;
    case BitOffsetRole:
        return node->bitOffset;
    case BitWidthRole:
        return node->bitWidth;
    case AbsoluteBitOffsetRole:
        return node->absoluteBitOffset;
    case OptionsRole:
        return node->options;
    case DependentOptionsRole:
        return node->dependentOptions;
    case DependsOnRole:
        return node->dependsOn;
    case EditableRole:
        return node->type != "reserved" && node->type != "nested";
    case ValueRole:
        return m_engine->getFieldValue(node->absoluteBitOffset, node->bitWidth);
    case DisplayValueRole:
        return getDisplayValue(node->absoluteBitOffset, node->bitWidth, node->options);
    }
    return {};
}

QHash<int, QByteArray> CodingTreeModel::roleNames() const
{
    return {
        {NameRole, "fieldName"},
        {TypeRole, "fieldType"},
        {BitOffsetRole, "bitOffset"},
        {BitWidthRole, "bitWidth"},
        {AbsoluteBitOffsetRole, "absoluteBitOffset"},
        {ValueRole, "fieldValue"},
        {DisplayValueRole, "displayValue"},
        {OptionsRole, "fieldOptions"},
        {DependentOptionsRole, "dependentOptions"},
        {DependsOnRole, "dependsOn"},
        {EditableRole, "editable"}
    };
}

void CodingTreeModel::refresh()
{
    beginResetModel();
    m_root = std::make_unique<TreeNode>();
    m_root->name = "root";
    addFieldNodes(m_root.get(), m_engine->fields(), 0);
    endResetModel();
}

void CodingTreeModel::setFieldValue(int absoluteBitOffset, int bitWidth, int value)
{
    m_engine->setFieldValue(absoluteBitOffset, bitWidth, value);
}

QString CodingTreeModel::getDisplayValue(int absoluteBitOffset, int bitWidth, const QVariantList &options) const
{
    int val = m_engine->getFieldValue(absoluteBitOffset, bitWidth);
    for (const auto &opt : options) {
        QVariantMap m = opt.toMap();
        if (m["value"].toInt() == val) {
            return m["label"].toString();
        }
    }
    return QString::number(val);
}

void CodingTreeModel::addFieldNodes(TreeNode *parent, const std::vector<CodingFieldDef> &fields, int parentAbsOffset)
{
    int row = 0;
    for (const auto &f : fields) {
        auto node = std::make_unique<TreeNode>();
        node->name = f.name;
        node->type = f.type;
        node->bitOffset = f.bitOffset;
        node->bitWidth = f.bitWidth;
        node->absoluteBitOffset = parentAbsOffset + f.bitOffset;
        node->options = f.options;
        node->dependentOptions = f.dependentOptions;
        node->dependsOn = f.dependsOn;
        node->parent = parent;
        node->row = row++;

        if (!f.children.empty()) {
            addFieldNodes(node.get(), f.children, node->absoluteBitOffset);
        }

        parent->children.push_back(std::move(node));
    }
}
