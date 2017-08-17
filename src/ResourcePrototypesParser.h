//
//  ResourceProrotypesParser.h
//  snowcrash
//
//  Created by Egor Baranov on 30/03/2017.
//

#ifndef SNOWCRASH_RESOURCEPROTOTYPESPARSER_H
#define SNOWCRASH_RESOURCEPROTOTYPESPARSER_H

#include "HeadersParser.h"
#include "ResourcePrototypeParser.h"

using namespace scpl;

namespace snowcrash {

    /** Resource prototypes matching regex */
    const char* const ResourcePrototypesRegex = "^[[:blank:]]*[Rr]esource[[:blank:]]+[Pp]rototypes[[:blank:]]*$";

    /**
     * Resource prototypes section processor
     */
    template<>
    struct SectionProcessor<ResourcePrototypes> : public SectionProcessorBase<ResourcePrototypes> {

        NO_SECTION_DESCRIPTION(ResourcePrototypes)

        static MarkdownNodeIterator processNestedSection(const MarkdownNodeIterator& node,
                                                         const MarkdownNodes& siblings,
                                                         SectionParserData& pd,
                                                         const ParseResultRef<ResourcePrototypes>& out) {

            IntermediateParseResult<ResourcePrototype> resourcePrototype(out.report);
            MarkdownNodeIterator cur = ResourcePrototypeParser::parse(node, siblings, pd, resourcePrototype);
            out.node.content.elements().push_back(resourcePrototype.node);
            
            /// TODO Source map?
            if (pd.exportSourceMap()) {
                SourceMap<Element> elementSM(Element::ResourcePrototypeElement);
                elementSM.content.resourcePrototype.name = resourcePrototype.sourceMap.name;
                elementSM.content.resourcePrototype.responses = resourcePrototype.sourceMap.responses;
                
                out.sourceMap.content.elements().collection.push_back(elementSM);
            }
            
            return cur;
        }

        static void finalize(const MarkdownNodeIterator& node,
                             SectionParserData& pd,
                             const ParseResultRef<ResourcePrototypes>& out) {
            
            out.node.element = Element::CategoryElement;
            out.node.category = Element::ResourcePrototypesGroupCategory;

            if (pd.exportSourceMap()) {
                
                out.sourceMap.element = out.node.element;
                out.sourceMap.category = out.node.category;
            }
        }

        static SectionType sectionType(const MarkdownNodeIterator& node) {
            if (node->type == mdp::HeaderMarkdownNodeType &&
                !node->text.empty()) {

                mdp::ByteBuffer remaining, subject = node->text;

                subject = GetFirstLine(subject, remaining);
                TrimString(subject);

                if (RegexMatch(subject, ResourcePrototypesRegex)) {
                    return ResourcePrototypesSectionType;
                }
            }

            return UndefinedSectionType;
        }

        static SectionType nestedSectionType(const MarkdownNodeIterator& node) {
            SectionType sectionType = SectionKeywordSignature(node);
            
            if (node->type == mdp::HeaderMarkdownNodeType &&
                sectionType == UndefinedSectionType) {
                
                return ResourcePrototypeSectionType;
            }
            
            return UndefinedSectionType;
        }

        static SectionTypes upperSectionTypes() {
            return {ResourcePrototypesSectionType, DataStructureGroupSectionType, ResourceGroupSectionType, ResourceSectionType};
        }
    };

    /** Resource prototypes Parser */
    typedef SectionParser<ResourcePrototypes, HeaderSectionAdapter> ResourcePrototypesParser;
}

#endif
