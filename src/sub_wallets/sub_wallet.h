// Copyright (c) 2018, The TurtleCoin Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <crypto/crypto.h>

#include "crypto_types.h"

#include <errors/errors.h>

#include "rapidjson/document.h"

#include <string>

#include <unordered_set>

#include "wallet_types.h"

class SubWallet
{
    public:

        //////////////////
        /* Constructors */
        //////////////////

        SubWallet() = default;

        SubWallet(
            const crypto::PublicKey publicSpendKey,
            const std::string address,
            const uint64_t scanHeight,
            const uint64_t scanTimestamp,
            const bool isPrimaryAddress);

        SubWallet(
            const crypto::PublicKey publicSpendKey,
            const crypto::SecretKey privateSpendKey,
            const std::string address,
            const uint64_t scanHeight,
            const uint64_t scanTimestamp,
            const bool isPrimaryAddress);

        /////////////////////////////
        /* Public member functions */
        /////////////////////////////

        /* Converts the class to a json object */
        void toJSON(rapidjson::Writer<rapidjson::StringBuffer> &writer) const;

        /* Initializes the class from a json string */
        void fromJSON(const JSONValue &j);

        /* Generates a key image from the derivation, and stores the
           transaction input along with the key image filled in */
        crypto::KeyImage getTxInputKeyImage( 
            const crypto::KeyDerivation derivation,
            const size_t outputIndex,
            const bool isViewWallet) const;
        
        /* Store a transaction input */
        void storeTransactionInput(
            const wallet_types::TransactionInput input,
            const bool isViewWallet);

        std::tuple<uint64_t, uint64_t> getBalance(
            const uint64_t currentHeight) const;

        void reset(const uint64_t scanHeight);

        bool isPrimaryAddress() const;

        std::string address() const;

        bool hasKeyImage(const crypto::KeyImage keyImage) const;

        crypto::PublicKey publicSpendKey() const;
        
        crypto::SecretKey privateSpendKey() const;

        void markInputAsSpent(
            const crypto::KeyImage keyImage,
            const uint64_t spendHeight);

        void markInputAsLocked(const crypto::KeyImage keyImage);

        void removeForkedInputs(const uint64_t forkHeight);

        void removeCancelledTransactions(const std::unordered_set<crypto::Hash> cancelledTransactions);

        /* Gets inputs that are spendable at the given height */
        std::vector<WalletTypes::TxInputAndOwner> getSpendableInputs(
            const uint64_t height) const;

        uint64_t syncStartHeight() const;

        uint64_t syncStartTimestamp() const;

        void storeUnconfirmedIncomingInput(
            const wallet_types::UnconfirmedInput input);

        void convertSyncTimestampToHeight(
            const uint64_t timestamp,
            const uint64_t height);

        /////////////////////////////
        /* Public member variables */
        /////////////////////////////

    private:

        /* A vector of the stored transaction input data, to be used for
           sending transactions later */
        std::vector<wallet_types::TransactionInput> m_unspentInputs;

        /* Inputs which have been used in a transaction, and are waiting to
           either be put into a block, or return to our wallet */
        std::vector<wallet_types::TransactionInput> m_lockedInputs;

        /* Inputs which have been spent in a transaction */
        std::vector<wallet_types::TransactionInput> m_spentInputs;

        /* Inputs which have come in from a transaction we sent - either from
           change or from sending to ourself - we use this to display unlocked
           balance correctly */
        std::vector<wallet_types::UnconfirmedInput> m_unconfirmedIncomingAmounts;

        /* This subwallet's public spend key */
        crypto::PublicKey m_publicSpendKey;

        /* The subwallet's private spend key */
        crypto::SecretKey m_privateSpendKey;

        /* The timestamp to begin syncing the wallet at
           (usually creation time or zero) */
        uint64_t m_syncStartTimestamp = 0;

        /* The height to begin syncing the wallet at */
        uint64_t m_syncStartHeight = 0;

        /* This subwallet's public address */
        std::string m_address;

        /* The wallet has one 'main' address which we will use by default
           when treating it as a single user wallet */
        bool m_isPrimaryAddress;
};
