#ifndef IRODS_FINALIZE_UTILITIES_HPP
#define IRODS_FINALIZE_UTILITIES_HPP

#include "irods/rodsType.h"

#include <string_view>

struct RsComm;
struct DataObjInfo;
struct DataObjInp;
struct l1desc;

namespace irods
{
    /// \brief Takes the ACLs included in the condInput and applies them to the objPath of _inp
    ///
    /// \param[in,out] _comm
    /// \param[in] _inp Supplies the objPath and condInput
    ///
    /// \since 4.2.9
    auto apply_acl_from_cond_input(RsComm& _comm, const DataObjInp& _inp) -> void;

    /// \brief Takes the metadata included in the condInput and applies them to the objPath of _inp
    ///
    /// \param[in,out] _comm
    /// \param[in] _inp Supplies the objPath and condInput
    ///
    /// \since 4.2.9
    auto apply_metadata_from_cond_input(RsComm& _comm, const DataObjInp& _inp) -> void;

    /// \brief Calls _dataObjChksum with the REGISTER_CHKSUM_KW
    ///
    /// \param[in,out] _comm
    /// \param[in,out] _info Required for _dataObjChksum
    /// \param[in] _original_checksum
    ///
    /// \returns the computed checksum
    ///
    /// \throws irods::exception If checksum operation fails for any reason
    ///
    /// \since 4.2.9
    auto register_new_checksum(RsComm& _comm, DataObjInfo& _info, std::string_view _original_checksum) -> std::string;

    /// \brief Calls _dataObjChksum with the VERIFY_CHKSUM_KW
    ///
    /// \param[in,out] _comm
    /// \param[in,out] _info Required for _dataObjChksum
    /// \param[in] _original_checksum
    ///
    /// \returns empty string if calculated checksum does not match the original; else, the computed checksum
    ///
    /// \throws irods::exception If checksum operation fails for any reason
    ///
    /// \since 4.2.9
    auto verify_checksum(RsComm& _comm, DataObjInfo& _info, std::string_view _original_checksum) -> std::string;

    /// \brief Calls getSizeInVault and verifies the result depending on the inputs
    ///
    /// If UNKNOWN_FILE_SZ is returned by getSizeInVault, the _recorded size is returned as
    /// the return value is likely the result of a resource which cannot stat its data in the
    /// expected manner. In this case, the result recorded by the plugin is "trusted".
    ///
    /// \param[in,out] _comm
    /// \param[in,out] _info
    /// \parma[in] _verify_size Verify the recorded size against the size in the vault
    /// \param[in] _recorded_size Size to verify against
    ///
    /// \returns If UNKNOWN_FILE_SZ is returned, _recorded_size; else, size in the vault
    ///
    /// \throws irods::exception If an error occurs getting the size or verification fails
    ///
    /// \since 4.2.9
    auto get_size_in_vault(RsComm& _comm, DataObjInfo& _info, const bool _verify_size, const rodsLong_t _recorded_size) -> rodsLong_t;

    /// \param[in] _src
    ///
    /// \returns Copy of _src L1 descriptor including DataObjInp and DataObjInfo
    ///
    /// \since 4.2.9
    auto duplicate_l1_descriptor(const l1desc& _src) -> l1desc;

    /// \brief Calls applyRule on the specified post-PEP name
    ///
    /// \param[in,out] _comm
    /// \param[in,out] _l1desc
    /// \param[in] _operation_status
    /// \param[in] _pep_name
    ///
    /// \returns REI status after calling applyRule
    ///
    /// \since 4.2.9
    auto apply_static_post_pep(RsComm& _comm, l1desc& _l1desc, const int _operation_status, std::string_view _pep_name) -> int;

    // TODO: ...remove this.
    auto purge_cache(RsComm& _comm, DataObjInfo& _info) -> int;

    /// \brief Call rs_replica_close without touching the catalog
    ///
    /// \param[in,out] _comm
    /// \param[in] _fd l1 descriptor index
    /// \param[in] _preserve_replica_state_table Whether the RST entry should be preserved or erased
    ///
    /// \returns return code of rs_replica_close
    ///
    /// \since 4.2.9
    auto close_replica_without_catalog_update(RsComm& _comm, const int _fd, const bool _preserve_replica_state_table) -> int;

    /// \brief Closes specified L1 descriptor and unlocks the data object (does not trigger file_modified)
    ///
    /// \param[in] _comm
    /// \param[in] _fd L1 descriptor to close
    /// \param[in] _status Desired status for the replica opened in _fd
    ///
    /// \returns return code of unlock_and_publish or close_replica_without_catalog_update
    ///
    /// \since 4.2.9
    auto close_replica_and_unlock_data_object(RsComm& _comm, const int _fd, const int _status) -> int;

    /// \brief Stales the specified replica status in the RST and publishes as-is
    ///
    /// \parblock
    /// This function should only be used in error cases. The call to irods::replica_state_table::publish::to_catalog
    /// is run with elevated privileges so that the unlock is guaranteed to work, barring issues with the database.
    ///
    /// In some instances, data_object_finalize may fail as business logic is attempting to unlock a data object. This presents
    /// a failure mode in which the replica state table has been updated but publishing to the catalog failed for some reason,
    /// which is of course an inconsistency. This function updates the specified replica's replica status in the RST to stale
    /// and publishes the rest of the RST entry to the catalog as-is, with elevated privileges.
    ///
    /// This utility assumes that the replica state table has been updated as desired. The target replica, if specified, will
    /// be marked stale and the other replica statuses will remain the same (again, this assumes that an unlock has already
    /// occurred).
    /// \endparblock
    ///
    /// \param[in,out] _comm
    /// \param[in] _ctx Context for publishing an RST entry (see irods::replica_state_table::publish::context for details)
    ///
    /// \returns Error code on failure
    ///
    /// \since 4.2.9
    auto stale_target_replica_and_publish(
        RsComm&          _comm,
        const rodsLong_t _data_id,
        const int        _replica_number) -> int;
} // namespace irods

#endif // IRODS_FINALIZE_UTILITIES_HPP
