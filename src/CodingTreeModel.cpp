#include "CodingTreeModel.h"
#include "CodingEngine.h"
#include "ApplicationConfig.h"
#include <QStringView>
#include <QDebug>

CodingTreeModel::CodingTreeModel(CodingEngine *engine, QObject *parent)
    : QAbstractItemModel(parent)
    , m_engine(engine)
{
    m_root = std::make_unique<TreeNode>();
    m_root->name = "root";

    connect(m_engine, &CodingEngine::configurationChanged, this, &CodingTreeModel::updateFieldValues);
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
    case EditableRole: {
        const auto &cfg = ApplicationConfig::instance();
        return node->type != cfg.defaultType && node->type != cfg.nestedType;
    }
    case ValueRole:
        return m_engine->getFieldValue(node->absoluteBitOffset, node->bitWidth);
    case DisplayValueRole:
        return getDisplayValue(node->absoluteBitOffset, node->bitWidth, node->options);
    }
    return {};
}

bool CodingTreeModel::hasChildren(const QModelIndex &parent) const
{
    TreeNode *node = parent.isValid()
        ? static_cast<TreeNode *>(parent.internalPointer())
        : m_root.get();
    
    return !node->children.empty();
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

void CodingTreeModel::updateFieldValues()
{
    // Emit dataChanged for all value-related roles to update display without collapsing
    for (int row = 0; row < rowCount(); ++row) {
        QModelIndex index = this->index(row, 0);
        emit dataChanged(index, index, {ValueRole, DisplayValueRole});

        updateChildValues(index);
    }
}

void CodingTreeModel::updateChildValues(const QModelIndex &parent)
{
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex childIndex = this->index(row, 0, parent);
        emit dataChanged(childIndex, childIndex, {ValueRole, DisplayValueRole});
        updateChildValues(childIndex);
    }
}

void CodingTreeModel::setFieldValue(int absoluteBitOffset, int bitWidth, int value)
{
    m_engine->setFieldValue(absoluteBitOffset, bitWidth, value);
}

QString CodingTreeModel::getDisplayValue(int absoluteBitOffset, int bitWidth, const QVariantList &options) const
{
    const int val = m_engine->getFieldValue(absoluteBitOffset, bitWidth);
    
    for (const auto &opt : options) {
        const QVariantMap m = opt.toMap();
        
        if (!m.contains("value")) {
            qWarning() << "Option missing 'value' field in getDisplayValue";
            continue;
        }
        
        bool ok = false;
        const int optionValue = m["value"].toInt(&ok);
        
        if (!ok) {
            qWarning() << "Failed to convert option value to int:" << m["value"];
            continue;
        }
        
        if (optionValue == val) {
            const QStringView labelView = m["label"].toString();
            return labelView.toString();
        }
    }
    
    return "0x" + QString::number(val, 16).toUpper().rightJustified(2, '0');
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
