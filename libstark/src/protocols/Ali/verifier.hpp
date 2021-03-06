#ifndef Ali_VERIFIER_HPP__
#define Ali_VERIFIER_HPP__

#include "protocols/protocol.hpp"
#include "common_details/common.hpp"
#include "languages/Acsp/AcspInstance.hpp"

#include "verifier_details/queriesGen.hpp"
#include "protocols/Fri/verifier.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "common/Utils/specsPrint.hpp"

#include <vector>
#include <functional>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Verifier{

typedef std::function<std::unique_ptr<IOPP_verifierInterface>(const std::vector<Algebra::FieldElement> evaluationBasis, const short securityLevel, const short logDegBound, const bool L0isMSB, specsPrinter& specsCollector)> RS_verifierFactory_t;

const RS_verifierFactory_t Biased_verifier = [](const std::vector<Algebra::FieldElement> evaluationBasis, const short securityLevel, const short logDegBound, const bool L0isMSB, specsPrinter& specsCollector){
    TASK("Generating verifier for RS proximity the 'BIASED' version");

    return std::unique_ptr<IOPP_verifierInterface>(new Protocols::Fri::Verifier::verifier_t(evaluationBasis, securityLevel, logDegBound, L0isMSB, specsCollector));
};

class verifier_t : public verifierInterface{
    public:
    verifier_t(const AcspInstance& instance, const RS_verifierFactory_t& RS_verifierFactory);
    void receiveMessage(const TranscriptMessage& msg);
    msg_ptr_t sendMessage(); 
    bool doneInteracting()const;
    bool verify()const;
    size_t expectedCommitedProofBytes()const;
    size_t expectedSentProofBytes()const;
    size_t expectedQueriedDataBytes()const;
    void fillResultsAndCommitmentRandomly();

    
    private:
    const AcspInstance& instance_;
    const Ali::details::randomCoeffsSet_t randCoeffs_;
    
    std::vector<details::linearCombinationValue> linearComb_witness_;
    std::vector<details::compositionWithZK_Value> polyEvaluation_composition_;
    Ali::details::partyState<uniEvalView_t> state_;
    std::unique_ptr<IOPP_verifierInterface> RS_verifier_witness_;
    std::unique_ptr<IOPP_verifierInterface> RS_verifier_composition_;
    Ali::details::phase_t phase_;

    //
    // methods
    //

    //verifies the Acsp has sound parameters, asserts otherwise
    static void verifyParams(const AcspInstance& instance);
    void generateRandomCoefficients(Ali::details::randomCoeffsSet_t& randCoeffs);
    void generateQueries(const RS_verifierFactory_t& RS_verifierFactory);
    
    void keepZK_Witness_maskCommitment(const CryptoCommitment::hashDigest_t& commitment);
    void keepZK_Composition_maskCommitment(const CryptoCommitment::hashDigest_t& commitment);
    void keepWitnessCommitment(const CryptoCommitment::hashDigest_t& commitment);
    
    void digestQueries(const queriesToInp_t& queriesToInput_witness, const queriesToInp_t& queriesToInput_composition);
    Ali::details::rawQueries_t getRawQueries()const;
    void digestResults(const Ali::details::rawResults_t& rawResults);
    bool verifyComitment()const;
    void fetchResults()const;
};


} // namespace Verifier
} // namespace Ali
} // namespace Protocols
} // namespace libstark

#endif //#ifndef Ali_VERIFIER_HPP__
