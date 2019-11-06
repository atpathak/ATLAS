#!/usr/bin/env python2

# please leave these two imports (used in doctest)
import argparse
import re
xyz = 42
# ok here you go

import sys
from QFramework import TQSampleFolder

from xmlparser import XMLParser, XMLNode, XMLGenericFactory
import ROOT
import itertools

class SequenceNotFoundException(Exception): pass
class UnknownIfOperationException(Exception): pass
class SchemaException(Exception): pass
class MissingAttribute(Exception): pass


class OTTNode(XMLNode):
    def __init__(self, *args, **kwds):
        """
        Create a OTTNode node and the root variable.
        """
        super(OTTNode, self).__init__(*args, **kwds)
        self.root = None

        
    def compile(self, context=None):
        """
        Compiles the node (and recursivlely all its sub nodes) by considering
        the context. The return value is a list of nodes, with which the node
        should be replaced in the document tree.

        The default OTTNode, applies the context by performing string
        replacements of it content, attributes and calling this function of
        all its children. This should modify the original node.

        >>> to = OTTNode("to", content="$(Frank)")
        >>> email = OTTNode("email", children=[to])
        >>> address_book = {"Frank": "f.sauerburger@cern.ch"}
        >>> email.children
        [OTTNode(name='to', content='$(Frank)')]
        >>> null = email.compile(address_book)
        >>> email.children
        [OTTNode(name='to', content='f.sauerburger@cern.ch')]
        """
        context = context or {}
                
        # string replacement in attributes
        for k, v in self.attributes.items():
            self.attributes[k] = self.apply_context(v, context)
        
        # string replacement in content
        self.content = self.apply_context(self.content, context)

        # replacing children with executed lists
        children = []
        for child in self.children:
            children.extend(child.compile(context))
        self.children = children

        return [self]

    def clone(self):
        """
        Creates a clone of itself (deep copy)
        """
        copy = self.__class__(self.name, content=self.content,
                attributes=dict(self.attributes))
        
        children = []
        for child in self.children:
            children.append(child.clone())
        copy.children = children

        copy.root = self.root

        return copy


    def apply_context(self, text, context):
        """
        Performs string replacement to compile the given string by considering
        the given context.
        >>> to = OTTNode("to")
        >>> address_book = {"Frank": "f.sauerburger@cern.ch"}
        >>> to.apply_context("$(Frank)", address_book)
        'f.sauerburger@cern.ch'
        >>> to.apply_context("$(NotFrank)", address_book)
        '$(NotFrank)'
        >>> multi = {"a": "$(b)", "b": "$(a)", "c": "$(c)"}
        >>> to.apply_context("$(a) $(b) $(c)", multi)
        '$(b) $(a) $(c)'
        """
        search = {"$(%s)" % k: v for k, v in context.items()}
        if search == {}:
            return text
        rx = re.compile('|'.join(map(re.escape, search)))
        def one_xlat(match):
            return search[match.group(0)]
        return rx.sub(one_xlat, text)

    def __str__(self):
        """
        Create a string representation with nice indend.
        >>> to = OTTNode("to", content="f.sauerburger@cern.ch")
        >>> email = OTTNode("email", children=[to])
        >>> str(email)
        '<email> \\n    <to> f.sauerburger@cern.ch'
        """
        first = "<%s> %s" % (" ".join([self.name] + 
            ["%s=\"%s\"" % x for x in self.attributes.items()]),
            self.content.replace("\n", " "))
        return "\n    ".join([first] + [str(x).replace("\n", "\n    ") for x in self.children])

    def set_root(self, root):
        """
        Sets the root variable of this node. This call is forwarded to all
        children. This is indented to make a node aware of the full xml tree
        and give it the possiblity to access other nodes.

        >>> to = OTTNode("to", content="f.sauerburger@cern.ch")
        >>> sender = OTTNode("sender", content="mail@example.com")
        >>> email = OTTNode("email", children=[to, sender])
        >>> email.set_root(email)
        >>> email.root == email
        True
        >>> sender.root == email
        True
        >>> to.root == email
        True
        """
        self.root = root
        map(lambda c: c.set_root(root), self.children)


class If(OTTNode):
    def compile(self, context=None):
        """
        Multiple conditions are joined with OR.

        Comiles the node. This means the for loop is executes.

        >>> parser = OTTParser()
        >>> email = parser.parse_string(test_xml)
        >>> email.set_root(email)
        >>> null = email.compile()
        >>> lines = email.get("body")[0].get("line")
        >>> len(lines)
        3
        >>> lines[0].content
        'checkout the image tree.png'
        >>> lines[1].content
        'checkout the really cool poem bird.txt'
        >>> lines[2].content
        'checkout the program lake.py'
        """
        context = context or {}

        # string replacement in attributes
        attributes = {}
        for k, v in self.attributes.items():
            attributes[k] = self.apply_context(v, context)

        expression = attributes["expression"]

        for attr, value in attributes.items():
            if attr == "expression":
                continue
            elif attr == "not-equals":
                if value != expression:
                    break
            elif attr == "equals":
                if value == expression:
                    break
            else:
                raise UnknownIfOperationException(attr)
        else:
            return []  # condition not fulfilled

        # break was triggered, i.e. compile if body
        children = []
        for child in self.children:
            children.extend(child.compile(context))
                
        return children
        
 
# sequences = {}
# batched_sequences = []
# class Sequence(OTTNode):
#     def __init__(self, *args, **kwds):
#         super(Sequence, self).__init__(*args, **kwds)
#         self.sequence_name = self.attributes["name"]
#         sequences[self.sequence_name] = self
#         if "batch" in self.attributes and \
#                 self.attributes["batch"] == "batch":
#             batched_sequences.append(self.sequence_name)
# 

    

            
            
        



class ForEach(OTTNode):
    def retrieve_sequence(self):
        """
        Returns the sequence over which the loop iterates.
        """
        if "each" not in self.attributes:
            raise SchemaException("<for> must have 'each' attribute.")
        if "as" not in self.attributes:
            raise SchemaException("<for> must have 'as' attribute.")

        sequence_name = self.attributes["each"]
        sequences = self.root.get("sequence")
        sequences = [s for s in sequences 
            if s.attributes["name"] == sequence_name]
        if len(sequences) == 0:
            raise SequenceNotFoundException(sequence_name)
        return  sequences[0]

    def compile(self, context=None):
        """
        Comiles the node. This means the for loop is executes.

        >>> parser = OTTParser()
        >>> email = parser.parse_string(test_xml)
        >>> email.set_root(email)
        >>> null = email.compile()
        >>> lines = email.get("body")[0].get("line")
        >>> len(lines)
        3
        >>> lines[0].content
        'checkout the image tree.png'
        >>> lines[2].content
        'checkout the program lake.py'
        """
        context = dict(context or {})
        
        # string replacement in attributes
        for k, v in self.attributes.items():
            self.attributes[k] = self.apply_context(v, context)


        # checking
        drop_high_low = "drop-high-low" in self.attributes and \
            self.attributes["drop-high-low"] == "drop-high-low"

        unique = "unique" in self.attributes and \
            self.attributes["unique"] == "unique"

        sequence = self.retrieve_sequence()

        # iterate sequence
        compiled_nodes = []
        used_string = set()
        for sequence_item in sequence:
            as_value = sequence_item.content
            as_name = self.attributes['as']

            # drop high and low suffixes
            if drop_high_low:
                if as_value.endswith("_high"):
                    as_value = as_value[:-5]
                elif as_value.endswith("_low"):
                    as_value = as_value[:-4]
            
            # loop only over unique values
            if as_value in used_string and unique:
                continue
            used_string.add(as_value)
                     
            # add as-value to context
            context[as_name] = as_value

            # add as.attribute-values to context
            for attr, value in sequence_item.attributes.items():
                variable_name = "%s.%s" % (self.attributes['as'], attr)
                context[variable_name] = value

            # compile all children
            for child in self.children:
                copy = child.clone()
                compiled_nodes.extend(copy.compile(context))

        return compiled_nodes

class Eval(OTTNode):
    def compile(self, context=None):
        """
        Compiles the node. This the expression is evaluated and added to the
        context.

        >>> parser = OTTParser()
        >>> email = parser.parse_string(test_xml)
        >>> email.set_root(email)
        >>> null = email.compile()
        >>> lumiininvpb = email.get("lumiininvpb")[0]
        >>> lumiininvpb.content
        '36455.95'
        """
        context = dict(context or {})
        
        # string replacement in attributes
        for k, v in self.attributes.items():
            self.attributes[k] = self.apply_context(v, context)

        # eval expression
        expression = self.attributes["expression"]
        value = eval(expression)

        # add value to context
        as_name = self.attributes["as"]
        context[as_name] = value

        # compile all children
        compiled_nodes = []
        for child in self.children:
            compiled_nodes.extend(child.compile(context))

        return compiled_nodes

class Sequence(OTTNode):
    def compile(self, context=None):
        """
        Takes only the pick-ths element if specified.
        >>> a = OTTNode("a")
        >>> b = OTTNode("b")
        >>> c = OTTNode("c")
        >>> seq = Sequence("sequence", children=[a, b, c], \
            attributes={"pick": "1"})
        >>> nope = seq.compile()
        >>> len(seq)
        1
        >>> seq[0].name
        'b'

        No number.
        >>> seq = Sequence("sequence", children=[a, b, c], \
            attributes={"pick": "None"})
        >>> nope = seq.compile()
        >>> len(seq)
        3

        Out of bound.
        >>> seq = Sequence("sequence", children=[a, b, c], \
            attributes={"pick": "15"})
        >>> nope = seq.compile()
        >>> len(seq)
        0
        """
        node, = super(Sequence, self).compile(context)

        if "pick" in self.attributes:
            try:
                n = int(self.attributes["pick"])
                node.children = [node.children[n]]
            except ValueError:
                # not a number (i.e. non-batch mode)
                pass
            except IndexError:
                # out of range
                node.children = []

        return [node]

class Print(OTTNode):
    """
    Simple instruction in the xml file to print an expression. This element
    will discard all children.
    """
    def compile(self, context=None):
        """
        Takes the expression and prints it to the terminal. All children will
        be discarded.
        >>> p = Print("print", attributes={"expression": "$(i)"})
        >>> context = {"i": "42"}
        >>> compiled = p.compile(context)
        PRINT: 42
        >>> compiled
        []
        """
        if not "expression" in self.attributes:
            raise MissingAttribute("expression")

        compiled = self.apply_context(self.attributes["expression"], context)
        print "PRINT:", compiled

        return []
        

class Load(ForEach):
    """
    Specialization of ForEach which iterates over an external file, instead of
    a sequence.

    Internally this class creates a floating (i.e. not attached to the xml
    tree) sequence and lets ForEach iterate over it.
    >>> parser = OTTParser()
    >>> email = parser.parse_string(test_xml)
    >>> email.set_root(email)
    >>> null = email.compile()
    >>> lakepy = email.get("attachment")[0]
    >>> lakepy.attributes["name"]
    'lake.py'
    >>> code_lines = lakepy.get("code")
    >>> len(code_lines) > 100
    True
    >>> code_lines[0]
    OTTNode(name='code', content='import')
    >>> code_lines[1]
    OTTNode(name='code', content='import')
    >>> code_lines[2]
    OTTNode(name='code', content='xyz')

    >>> lumi = email.get("luminosity")[0]
    >>> lumi.content
    '36.45595'
    """
    def load_text(self, file_name):
        """
        Loads a text file an returns the results as nested lists. Each line
        will be converted represented by on item in the outer list. The line
        is the subdevided into an other list on whitespaces.

        Lines whoses first non-whitespace character is a hash (#), will be
        irgnored. Lines without non-whitespace characters are also ignored.
        >>> l = Load("load")
        >>> lines = l.load_text("ott.py")
        >>> lines[0]
        ['import', 'argparse']
        >>> lines[1]
        ['import', 're']
        """
        lines = []
        with open(file_name) as file:
            for line in file:
                cols = line.split()
                if len(cols) == 0:
                    continue
                if cols[0].startswith("#"):
                    continue
                lines.append(cols)
        return lines

    def parse_qconfig(self, config_file):
        """
        Parse a qframework config file.
        """
        config = { }
        with open(config_file) as f:
          for line in f:
            m = re.match(r'([\w.-]+):\s*([^#]*)\s*', line)
            if m:
              config[m.group(1).strip()] = m.group(2).strip()
        return config

    def retrieve_sequence(self):
        """
        Retrive the sequence over which ForEach iterates. This sequence is
        build from the text file.
        >>> l = Load("load", attributes={"txt": "ott.py"})
        >>> code_seq = l.retrieve_sequence()
        >>> code_seq[0]
        OTTNode(name='item', content='import', attributes={'col-0': 'argparse'})
        >>> code_seq[1]
        OTTNode(name='item', content='import', attributes={'col-0': 're'})
        >>> code_seq[2]
        OTTNode(name='item', content='xyz', attributes={'col-0': '=', 'col-1': '42'})
        """
        if "txt" in self.attributes:
            lines = self.load_text(self.attributes["txt"])
            items = [OTTNode("item", content=l[0],
                attributes={"col-%d" % i: v for i, v in enumerate(l[1:])})
                for l in lines]
        elif "qconfig" in self.attributes:
            config = self.parse_qconfig(self.attributes["qconfig"]) 
            items = [OTTNode("item", content=config[self.attributes["option"]])]

        sequence = OTTNode("sequence", children=items)
        return sequence  

class OTTParser(XMLParser):
    def __init__(self, *args, **kwds):
        super(OTTParser, self).__init__(*args, **kwds)
        self.default_factory = XMLGenericFactory(OTTNode)
        self.add_factory("for", XMLGenericFactory(ForEach))
        self.add_factory("load", XMLGenericFactory(Load))
        self.add_factory("if", XMLGenericFactory(If))
        self.add_factory("eval", XMLGenericFactory(Eval))
        self.add_factory("sequence", XMLGenericFactory(Sequence))
        self.add_factory("print", XMLGenericFactory(Print))


def index_to_manyi(sequences, batched_sequences, batch_index):
    bases = [len(sequences[name]) for name in batched_sequences]
    for k in range(len(bases) - 1, 0, -1):
        bases[k-1] *= bases[k]

    bases = bases[1:] + [1]

    manyi = []
    for b in bases:
        i = batch_index / b
        manyi.append(i)
        batch_index -= i * b

    return manyi


def main(transformation, p, batch_index):
    ott_parser = OTTParser()
    transformation = ott_parser.parse_file(transformation)
    transformation.set_root(transformation)
    batch_index = str(batch_index) or "all"
    transformation.compile({"-batch-index": batch_index})

    if p:
        print transformation

    for input in transformation.get("input"):
        sf = False
        print "Open: %s" % input.attributes['path']
        if "sample-folder" in input.attributes:
            input_file = TQSampleFolder.loadSampleFolder(
                "%s:%s" % (input.attributes['path'], input.attributes['sample-folder']))
            sf = True
        else:
            input_file = ROOT.TFile.Open(input.attributes['path'])

        for output in transformation.get("output"):
            output_file = ROOT.TFile.Open(output.attributes['path'], "RECREATE")

            for TH1F in output.get("TH1F"):
                output_file.cd()
                min = float(TH1F.get("min")[0].content)
                max = float(TH1F.get("max")[0].content)
                destination = TH1F.get("destination")[0].content
                bins = TH1F.get("bin")

                token = destination.rsplit("/", 1)
                if len(token) == 1:
                    token = "", token[0]

                if not output_file.Get(token[0]):
                    output_file.mkdir(token[0])

                output_file.cd(token[0])
                destination_name = token[1]

                print "  TH1F(\"%s\", \"\", %d, %g, %g) -> %s  %s" % \
                    (destination_name, len(bins), min, max,
                    output.attributes['path'], destination)

                histogram = ROOT.TH1F(destination_name, "", len(bins), min, max)
                for i, bin in enumerate(bins):
                    histogram.SetBinContent(i + 1, float(bin.content))
                histogram.Write()

            for duplicate in output.get("duplicate"):
                output_file.cd()
                source = duplicate.get("source")[0].content
                destination = duplicate.get("destination")[0].content

                token = destination.rsplit("/", 1)
                if len(token) == 1:
                    token = "", token[0]

                if not output_file.Get(token[0]):
                    output_file.mkdir(token[0])

                output_file.cd(token[0])
                destination_name = token[1]

                if sf:
                    cut = duplicate.get("cut")[0].content
                    histogram_name = duplicate.get("histogram")[0].content


                    histogram = input_file.getHistogram(ROOT.TString(source),
                        ROOT.TString("%s/%s" %  (cut, histogram_name)))
                    if not histogram:
                        continue
                    print "  %s  %s/%s -> %s  %s" % \
                        (source, cut, histogram_name, 
                        output.attributes['path'], destination)
                    clone = histogram.Clone(destination_name)
                    clone.Write()
                # todo

            output_file.Close()






if __name__ == "__main__":
        
    parser = argparse.ArgumentParser("OTT - Output Transformation Tool.")
    parser.add_argument("transformation",
        help="OOT transformation in xml format.")

    parser.add_argument("--print", "-p", dest="p", action="store_true",
        help="Print the compiled transformation structure.")

    parser.add_argument("--batch-index", "-i", nargs="?", type=int,
        help="Print the compiled transformation structure.")

    args = parser.parse_args()
    main(**vars(args))








# example xml used for unittesting
test_xml = """<?xml version="1.0"?>
<email id="1343">
    <to>f.sauerburger@cern.ch</to>
    <from>mail@example.com</from>
    <subjets>have you heart about unittests?</subjets>
    <sequence name="attachments">
        <item type="image" emph="False">tree.png</item>
        <item type="poem" emph="True">bird.txt</item>
        <item type="program" emph="False">lake.py</item>
    </sequence>
    <body>
        <for each="attachments" as="attachment">
            <if expression="$(attachment.emph)" not-equals="True">
                <line>checkout the $(attachment.type) $(attachment)</line>
            </if>
            <if expression="$(attachment.emph)" equals="True">
                <line>checkout the really cool $(attachment.type) $(attachment)</line>
            </if>
        </for>
    </body>
    <attachment name="lake.py">
         <load txt="ott.py" as="sys">
             <code>$(sys)</code>
         </load>
    </attachment>
    <load qconfig="../config/htt_lephad_fake.cfg" option="makeSampleFile.luminosity" as="lumi">
        <luminosity>$(lumi)</luminosity>
    </load>
    <load qconfig="../config/htt_lephad_fake.cfg" option="makeSampleFile.luminosity" as="lumi">
        <eval expression="str(float($(lumi)) * 1000)" as="lumi">
            <lumiininvpb>$(lumi)</lumiininvpb>
        </eval>
    </load>
    
    <!-- sequences can be build dynamically -->
    <sequence name="ott">
        <load txt="ott.py" as="sys">
            <item>$(sys)</item>
        </load>
    </sequence>
    <hack name="lake.py">
         <for each="ott" as="syss">
             <code>$(syss)</code>
         </for>
    </hack>
</email>
"""
