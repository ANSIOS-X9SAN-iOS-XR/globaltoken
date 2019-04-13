// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The DigiByte Core developers
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GLOBALTOKEN_BLOCK_DEPENDENCIES_H
#define GLOBALTOKEN_BLOCK_DEPENDENCIES_H

#include <serialize.h>
#include <uint256.h>

/**
 * Pure Version that will inherit to all other Block classes
 * Includes nVersion and AuxPow stuff.
 */
class CPureBlockVersion
{
private:

    /* Modifiers to the version.  */
    static const int32_t VERSION_AUXPOW = (1 << 8);

    /** Bits above are reserved for the auxpow chain ID.  */
    static const int32_t VERSION_CHAIN_START = (1 << 16);
    
public:
    // header
    int32_t nVersion;

    CPureBlockVersion()
    {
        SetNull();
    }
    
    CPureBlockVersion(int32_t nBlockVersion)
    {
        nVersion = nBlockVersion;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(this->nVersion);
    }

    void SetNull()
    {
        nVersion = 0;
    }

    /* Below are methods to interpret the version with respect to
       auxpow data and chain ID.  This used to be in the CBlockVersion
       class, but was moved here when we switched back to nVersion being
       a pure int member as preparation to undoing the "abuse" and
       allowing BIP9 to work.  */

    /**
     * Extract the base version (without modifiers and chain ID).
     * @return The base version./
     */
    inline int32_t GetBaseVersion(int32_t nChainId) const
    {
        return GetBaseVersion(nVersion, nChainId);
    }
    
    static inline int32_t GetBaseVersion(int32_t ver, int32_t nChainId)
    {
        int32_t version = ver;
        //return ver % VERSION_AUXPOW;
        if(version & VERSION_AUXPOW)
            version = version ^ VERSION_AUXPOW;
        
        return version ^ (nChainId * VERSION_CHAIN_START);
    }

    /**
     * Set the base version (apart from chain ID and auxpow flag) to
     * the one given.  This should only be called when auxpow is not yet
     * set, to initialise a block!
     * @param nBaseVersion The base version.
     * @param nChainId The auxpow chain ID.
     */
    void SetBaseVersion(int32_t nBaseVersion, int32_t nChainId);

    /**
     * Extract the chain ID.
     * @return The chain ID encoded in the version.
     */
    inline int32_t GetChainId() const
    {
        return nVersion / VERSION_CHAIN_START;
    }

    /**
     * Set the chain ID.  This is used for the test suite.
     * @param ch The chain ID to set.
     */
    inline void SetChainId(int32_t chainId)
    {
        nVersion %= VERSION_CHAIN_START;
        nVersion |= chainId * VERSION_CHAIN_START;
    }

    /**
     * Check if the auxpow flag is set in the version.
     * @return True iff this block version is marked as auxpow.
     */
    inline bool IsAuxpow() const
    {
        return nVersion & VERSION_AUXPOW;
    }

    /**
     * Set the auxpow flag.  This is used for testing.
     * @param auxpow Whether to mark auxpow as true.
     */
    inline void SetAuxpowVersion (bool auxpow)
    {
        if (auxpow)
            nVersion |= VERSION_AUXPOW;
        else
            nVersion &= ~VERSION_AUXPOW;
    }
    
    /**
     * Extract the normal version from an Auxpow block.
     * It is needed to validate the algo.
     * @return The basic Version without auxpow.
     */
    inline int32_t GetAuxpowVersion() const
    {
        if(IsAuxpow())
            return nVersion ^ VERSION_AUXPOW;
        else
            return nVersion;
    }
    
    /**
     * Checks if the Blockversion is a legacy version (non-hardfork).
     * @param nVersion the block version to check.
     * @return true if it is legacy, false if not.
     */
    inline bool IsLegacyVersion(int32_t blockversion) const
    {
        return (blockversion == 1 || blockversion == 2 || blockversion == 536870912 || blockversion == 536870913 || blockversion == 536870914);
    }
};

#endif // GLOBALTOKEN_BLOCK_DEPENDENCIES_H
