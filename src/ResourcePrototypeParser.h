//
//  ResourceProrotypesParser.h
//  snowcrash
//
//  Created by Egor Baranov on 30/03/2017.
//

#ifndef SNOWCRASH_RESOURCEPROTOTYPEPARSER_H
#define SNOWCRASH_RESOURCEPROTOTYPEPARSER_H

#include "HeadersParser.h"

using namespace scpl;

namespace snowcrash {

    /**
     * Resource prototype section processor
     */
    template<>
    struct SectionProcessor<ResourcePrototype> : public SignatureSectionProcessorBase<ResourcePrototype> {

        NO_SECTION_DESCRIPTION(ResourcePrototype)

        static SignatureTraits signatureTraits() {

            SignatureTraits signatureTraits(SignatureTraits::IdentifierTrait |
                                            SignatureTraits::AttributesTrait);

            return signatureTraits;
        }

        static MarkdownNodeIterator finalizeSignature(const MarkdownNodeIterator& node,
                                                      SectionParserData& pd,
                                                      const Signature& signature,
                                                      const ParseResultRef<ResourcePrototype>& out) {

            parseResourcePrototypeDefinition(signature, out.node.content.resourcePrototypeDefinition);
            ResourcePrototypeDefinition& proto = out.node.content.resourcePrototypeDefinition;

            if (pd.exportSourceMap()) {

                if (!proto.name.empty()) {
                    out.sourceMap.name.sourceMap = node->sourceMap;
                }
            }

            return ++MarkdownNodeIterator(node);
        }

        static MarkdownNodeIterator processNestedSection(const MarkdownNodeIterator& node,
                                                         const MarkdownNodes& siblings,
                                                         SectionParserData& pd,
                                                         const ParseResultRef<ResourcePrototype>& out) {

            // TODO Implement this method correctly
            MarkdownNodeIterator cur = node;
            switch (pd.sectionContext()) {
                case ResponseSectionType:
                case ResponseBodySectionType:
                {
                    IntermediateParseResult<Payload> payload(out.report);

                    cur = PayloadParser::parse(node, siblings, pd, payload);

                    out.node.content.resourcePrototypeDefinition.responses.push_back(payload.node);

                    if (pd.exportSourceMap()) {
                        out.sourceMap.responses.collection.push_back(payload.sourceMap);
                    }

                    break;
                }

                default:
                    break;
            }

            return cur;
        }

        static void finalize(const MarkdownNodeIterator& node,
                             SectionParserData& pd,
                             const ParseResultRef<ResourcePrototype>& out) {
            out.node.element = Element::ResourcePrototypeElement;
        }

        static SectionType sectionType(const MarkdownNodeIterator& node) {

            SectionType sectionType = SectionKeywordSignature(node);

            if (node->type == mdp::HeaderMarkdownNodeType &&
                sectionType == UndefinedSectionType) {

                return ResourcePrototypeSectionType;
            }

            return UndefinedSectionType;
        }

        static SectionType nestedSectionType(const MarkdownNodeIterator& node) {
            SectionType result = SectionProcessor<Action>::nestedSectionType(node);
            if (result == UndefinedSectionType && SectionKeywordSignature(node) == UndefinedSectionType) {
                result = ResourcePrototypeSectionType;
            }

            return result;
        }

        static SectionTypes upperSectionTypes() {
            return {ResourcePrototypesSectionType, ResourcePrototypeSectionType, DataStructureGroupSectionType, ResourceGroupSectionType, ResourceSectionType};
        }

        static void parseResourcePrototypeDefinition(const Signature& signature, ResourcePrototypeDefinition& typeDefinition) {
            typeDefinition.name = signature.identifier;
            if (signature.attributes.size() > 0) {
                typeDefinition.baseName = signature.attributes[0];
            }
        }
    };

    /** Resource prototype Parser */
    typedef SectionParser<ResourcePrototype, HeaderSectionAdapter> ResourcePrototypeParser;
}

#endif
