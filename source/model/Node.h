#ifndef SHADERGRAPH_NODE_H
#define SHADERGRAPH_NODE_H

#include <list>
#include <vector>
#include <string>

#include <QVBoxLayout>
#include <QLayoutItem>
#include <QLayout>

// WIP

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <core/Core.h>

#include "Data.h"
#include "Interface.h"

#define WIDGET_NODE_SIZE    75
#define IMAGE_NODE_SIZE     150


#define GLSL_CODE(_code_, ...) \
do \
{ \
    fmt::memory_buffer membuf; \
    format_to(membuf, __VA_ARGS__); \
    _code_ = to_string(membuf); \
} while (false); \

namespace ShaderGraph
{
    /// The node ID giver.
    static unsigned int g_nodeId = 0;
    #define GET_NEW_NODE_ID g_nodeId++

    using QtNodes::NodeValidationState;

    class Node : public QtNodes::NodeDataModel
    {
        Q_OBJECT

    public:
        /// Default constructor.
        Node() : m_id(GET_NEW_NODE_ID) {}

        /// Constructor.
        explicit Node(QString name) :
            m_name(name),
            m_caption(name),
            m_id(GET_NEW_NODE_ID)
        {

        }

        /// Constructor.
        Node(QString name, QString caption) :
            m_name(name),
            m_caption(caption),
            m_id(GET_NEW_NODE_ID)
        {

        }

        /// Default destructor.
        ~Node() override = default;

        /* ============================== Nodeeditor - Node props ============================== */

        /// Give for a specified port, the number of data.
        /// @portType : the type of the port.
        /// @return : the number of the data.
        unsigned int nPorts(QtNodes::PortType portType) const override;

        /// Give for a specified port, its type.
        /// @portType : the type of the port.
        /// @index : the index of the port.
        /// @return : the type of the data.
        QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex index) const override;

        /// That event happens when an other node is plug-in.
        /// @data : the plugged-in data node.
        /// @index : the index of the port.
        void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex index) override;

        /// The output of this node, at the specified index.
        /// @index : the index of the port.
        /// @return : a shared point to the data to retrieve.
        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex index) override;

        /* ============================== Nodeeditor - Node data ============================== */

        /// Specified the embedded widget in the Node.
        /// @return : the widget.
        QWidget * embeddedWidget() override;

        /// Getter only to the reference to the name.
        QString name() const override;

        /// Getter to the reference to the caption.
        QString caption() const override;

        /* ============================== Nodeeditor - Node error handler ============================== */

        /// Getter : Current state of this node.
        NodeValidationState validationState() const override { return m_validationState; }

        /// Getter : The error message of this node.
        QString validationMessage() const override { return m_validationMessage; }

        /* ============================== Code Generation ============================== */

        /// Generate the name of @pin in the GLSL code.
        /// Format : nodeID<nodeID>_<pinname>, with :
        ///     <nodeID> : The id of this node
        ///     <pinname>: The name of the pin, with the convention : lowercase.
        std::string autoName(PIN pin);

        /// Generate the declarations of this node outputs, for the GLSL code.
        std::string outputsToGLSL();

        /// Generate the declarations + initialization of this node inputs, for the GLSL code.
        /// @nodes : A list of nodeID, use to store the visited/generated nodes.
        std::string inputsToGLSL(std::list<unsigned int>& nodes);

        /// Generate the "core" of this node, for the GLSL code.
        virtual std::string nodeToGLSL() = 0;

        /// Generate the equivalent in GLSL code.
        /// @warning : Will generate the node that it depends on.
        std::string toGLSL();

        /// Generate the equivalent in GLSL code.
        /// @warning : Will generate the node that it depends on.
        /// @nodes : A list of nodeID, use to store the visited/generated nodes.
        std::string toGLSL(std::list<unsigned int> nodes);

        /* ============================== Details ============================== */

        /// Show all node's properties in the node panel.
        virtual void showDetails(QTreeWidget * tree);

        /// Hide all node's properties from the node panel.
        virtual void hideDetails(QTreeWidget * tree);

        /// @return : true if this node is showing its properties.
        inline bool isDetailedNode() const { return m_isDetailedNode; }

        /* ============================== Getter/Setter ============================== */

        /// Getter : The node ID.
        inline unsigned int getID() const { return m_id; };

        /// Getter to the reference to a vector of inputs.
        // TODO : Make sure that every PIN name, in @m_inputs and @m_details, are unique.
        //        Needed by autoName.
        std::vector<PIN>& inputs()  { return m_inputs;  }

        /// Getter to the reference to a vector of outputs.
        // TODO : Make sure that every PIN name in @m_outputs are unique.
        //        Needed by autoName.
        std::vector<PIN>& outputs() { return m_outputs; }

        /// Getter to the reference to a vector of details.
        // TODO : Make sure that every PIN name, in @m_inputs and @m_details, are unique.
        //        Needed by autoName.
        std::vector<PIN>& details() { return m_details; }

      protected:

        /// Setter : Details panel layout
        inline void setDetailsPanelLayout(QVBoxLayout *layout) { m_detailsPanelLayout = layout; }

        /// Setter : The index of the details layout in the previous function.
        inline void setDetailsPanelIndexLayout(unsigned int idx)
        {
          m_detailsPanelIndexLayout = idx;
          m_isDetailsPanelLayoutInit = true;
        }

        /// Update the validation state of this node.
        /// @warning : if the node is valid, no message will be displayed.
        inline void updateNodeValidation(NodeValidationState state, QString message = "No message")
        {
            m_validationState = state;
            m_validationMessage = message;
        }

    private:
        // TODO : comment me :)
        QVBoxLayout * m_detailsPanelLayout = nullptr;

        // TODO : comment me :)
        size_t m_detailsPanelIndexLayout = 0;

        // TODO : comment me :)
        bool m_isDetailsPanelLayoutInit = false;


        bool m_isDetailedNode = false;

        /// The name of the node or what will be displayed on the screen.
        QString m_name;

        /// The caption or a brief description.
        QString m_caption;

        /// The ID of this node.
        /// Each node has a unique id.
        unsigned int m_id;

        /// The layer of this node.
        /// A layer is the max between the value of each.
        unsigned int m_layer = 0;

        /// The internal representation of the input pins.
        std::vector<PIN> m_inputs;

        /// The internal representation of the output pins.
        std::vector<PIN> m_outputs;

        /// The hidden properties.
        std::vector<PIN> m_details;

        /// The error/warning message. "No message" if no error/warning.
        QString m_validationMessage = "No message";

        /// The state (valid, warning, error) state of this node.
        NodeValidationState m_validationState = NodeValidationState::Valid;
    };
}

#endif //SHADERGRAPH_NODE_H
