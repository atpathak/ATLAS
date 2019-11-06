#!/usr/bin/env python2

import ROOT

class RootElementNotSingularException(Exception):
    pass

class XMLNode(object):
    """
    Representation of a xml node.
    """
    def __init__(self, name, content=None, attributes=None, children=None):
        """
        Constructor of a base XMLNode. All the information of the node has to
        be passed in via parameters.

        The constructor can only be used to create a single node.
        >>> node = XMLNode(name="body", content="Hi Frank.", \
         attributes={"signed": "True"})
        >>> node.name, node.content, node.attributes, node.children
        ('body', 'Hi Frank.', {'signed': 'True'}, [])
        """
        self.name = name
        self.content = content or ""
        self.attributes = attributes or {}
        self.children = children or []

    def __getitem__(self, index):
        """
        Use [] to access the children.
        >>> to = XMLNode("to", content="f.sauerburger@cern.ch")
        >>> sender = XMLNode("sender", content="mail@example.com")
        >>> email = XMLNode("email", children=[to, sender])
        >>> email[0] == to
        True
        >>> email[1] == sender
        True
        """
        return self.children[index]

    def __contains__(self, value):
        """
        Checks wether a node is among its children.
        >>> to = XMLNode("to", content="f.sauerburger@cern.ch")
        >>> sender = XMLNode("sender", content="mail@example.com")
        >>> email = XMLNode("email", children=[to, sender])
        >>> to in email
        True
        >>> sender in email
        True
        >>> sender in to
        False
        >>> email in to
        False
        """
        return (value in self.children)

    def __len__(self):
        """
        Length of the children list.
        >>> to = XMLNode("to", content="f.sauerburger@cern.ch")
        >>> sender = XMLNode("sender", content="mail@example.com")
        >>> email = XMLNode("email", children=[to, sender])
        >>> len(email)
        2
        >>> len(to)
        0
        """
        return len(self.children)

    def __repr__(self):
        """
        Representation of a node with its content, children and attributes.
        >>> to = XMLNode("to", content="f.sauerburger@cern.ch")
        >>> email = XMLNode("email", children=[to])
        >>> to
        XMLNode(name='to', content='f.sauerburger@cern.ch')
        >>> email
        XMLNode(name='email', children=[XMLNode(name='to', content='f.sauerburger@cern.ch')])
        """
        args = []
        args.append("name=%s" % repr(self.name))

        if self.content != "":
            args.append("content=%s" % repr(self.content))

        if self.attributes != {}:
            args.append("attributes=%s" % repr(self.attributes))

        if self.children != []:
             args.append("children=%s" % repr(self.children))

        clas = XMLNode
        if issubclass(self.__class__, XMLNode):
            cls = self.__class__.__name__

        return "%s(%s)" % (cls, ", ".join(args))

    def get(self, node_name):
        """
        Return a list of all children with this node_name.
        >>> email1 = XMLNode("email")
        >>> email2 = XMLNode("email")
        >>> other = XMLNode("other")
        >>> all = XMLNode("containter", children=[email1, email2, other])
        >>> emails = all.get("email")
        >>> len(emails)
        2
        >>> email1 in emails
        True
        >>> email2 in emails
        True
        """
        return [c for c in self.children if c.name == node_name]
     
class XMLGenericFactory(object):
    """
    Generic factory to create nodes. The node class is determined via an
    argument of the constructor. The create methods return a list of nodes.
    This means a single node in the xml file be expanded to serveral nodes
    when created with certain factories. This allows for static loop execution
    during parsing. Static in this context means, the xml tree is not
    available to the factory.
    """
    def __init__(self, node_class):
        """
        Creates a generic factory, which creates nodes with the given class.
        >>> class Address(XMLNode): pass
        >>> address_factory = XMLGenericFactory(Address)
        >>> address_factory.create_from_args("to", content="f.sauerburger@cern.ch")
        [Address(name='to', content='f.sauerburger@cern.ch')]
        """
        self.node_class = node_class

    def create_from_args(self, name, content=None, attributes=None, children=None):
        """
        Create node(s) based on the parameters. This is similar to the
        constructor of XMLNode, except it takes the factory specific class and
        returns the node wrapped in a list.
            
        Other factories might implemenet a feature such that, the a node is
        executed/load/expanded and replaced by (an) other node(s). 

        >>> class Address(XMLNode): pass
        >>> address_factory = XMLGenericFactory(Address)
        >>> address_factory.create_from_args("to", content="f.sauerburger@cern.ch")
        [Address(name='to', content='f.sauerburger@cern.ch')]
        """
        node = self.node_class(name, content, attributes, children)
        return [node]


    def create_from_xml(self, parser, p_node):
        """
        Parses the node p_node using the xml engine. This also recursivly
        creates all children.

        When this method is called the resulting object will be of type
        node_class. The node_name is not used to retrieve an other factory.
        However get_factory is called for each child node.

        (implicitly tested)
        """
        name = parser.engine.GetNodeName(p_node)
        content = parser.engine.GetNodeContent(p_node)

        attributes = {}
        p_attr = parser.engine.GetFirstAttr(p_node)
        while p_attr != ROOT.nullptr:
            attr_name = parser.engine.GetAttrName(p_attr)
            attr_value = parser.engine.GetAttrValue(p_attr)
            attributes[attr_name] = attr_value
            p_attr = parser.engine.GetNextAttr(p_attr)

        children = []
        p_child = parser.engine.GetChild(p_node)
        while p_child != ROOT.nullptr:
            child_name = parser.engine.GetNodeName(p_child) 
            child_factory = parser.get_factory(child_name)
            expanded_children = child_factory.create_from_xml(parser, p_child)
            children.extend(expanded_children)
            p_child = parser.engine.GetNext(p_child)

        # this factory creates the node (does not call get_factory!)
        nodes = self.create_from_args(name, content, attributes, children)
        return nodes

class XMLParser(object):
    """
    Main class of this module. Has functionality to convert xml to object
    structure.
    """ 
    def parse_file(self, file_name):
        """
        Parses an xml file and returns the parsed object structure.
        """
        doc = self.engine.ParseFile(file_name)
        return self.parse(doc)

    def parse_string(self, string):
        """
        Parse an xml document and returns the parsed object structure.

        >>> parser = XMLParser()
        >>> email = parser.parse_string(test_xml)
        >>> len(email.get("to"))
        1
        >>> email.get("to")[0].content
        'f.sauerburger@cern.ch'
        >>> len(email.get("body"))
        1
        >>> email.get("body")[0].attributes["signed"]
        'True'
        >>> email.attributes["id"]
        '1343'
        """
        doc = self.engine.ParseString(string)
        return self.parse(doc)

    def parse(self, doc):
        """
        Internal usage only. Use parse_file() and parse_string()

        Parses an TXMLEngine document object and returns the parsed object
        structure. 
        """
        p_root = self.engine.DocGetRootElement(doc)
        name = self.engine.GetNodeName(p_root)
        factory = self.get_factory(name)
        root = factory.create_from_xml(self, p_root)
        if len(root) != 1:
            raise RootElementNotSingularException(root)
        return root[0]
    
    def __init__(self):
        """
        Create a xml parser.

        >>> parser = XMLParser()
        >>> email = parser.parse_string(test_xml)
        >>> email.attributes["id"]
        '1343'
        >>> parser.factories
        {}
        >>> isinstance(parser.default_factory, XMLGenericFactory)
        True
        """
        self.engine = ROOT.TXMLEngine()
        self.factories = {}
        self.default_factory = XMLGenericFactory(XMLNode)

    def add_factory(self, node_name, factory):
        """
        Adds (overwrites existing) factory associated with xml nodes given by
        node_name.
        >>> class Address(XMLNode): pass
        >>> parser = XMLParser()
        >>> parser.add_factory("to", XMLGenericFactory(Address))
        >>> parser.add_factory("from", XMLGenericFactory(Address))
        >>> "to" in parser.factories
        True
        >>> "from" in parser.factories
        True
        >>> email = parser.parse_string(test_xml)
        >>> email.get("to")[0]
        Address(name='to', content='f.sauerburger@cern.ch')
        >>> email.get("from")[0]
        Address(name='from', content='mail@example.com')
        """
        self.factories[node_name] = factory

    def get_factory(self, node_name):
        """
        Return the factory associated with node_name. If no factory is
        associated with this node name, return the default factory.
        >>> parser = XMLParser()
        >>> parser.add_factory("to", "btw: this string is not a factory...")
        >>> parser.get_factory("to")
        'btw: this string is not a factory...'
        >>> isinstance(parser.get_factory("xxx"), XMLGenericFactory)
        True
        """
        if node_name in self.factories:
            return self.factories[node_name]
        else:
            return self.default_factory

# example xml used for unittesting
test_xml = """<?xml version="1.0"?>
<email id="1343">
    <to>f.sauerburger@cern.ch</to>
    <from>mail@example.com</from>
    <subjets>have you heart about unittests?</subjets>
    <body signed="True"> see subject</body>
</email>
"""
