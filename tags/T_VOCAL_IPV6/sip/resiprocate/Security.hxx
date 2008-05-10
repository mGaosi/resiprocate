#if !defined(SECURITY_HXX)
#define SECURITY_HXX


#ifdef USE_SSL
#include <openssl/evp.h>
#include <openssl/x509.h>
#endif


namespace Vocal2
{
 
class Contents;
class Pkcs7Contents;

class Security
{
   public:
      Security( );
      ~Security();
      
      /* The security stuff is for managing certificates that are used by the
       * S/MIME and TLS stuff. Filenames can be provided for all these items but
       * the default asumption if no filename is provided is that the item can
       * be loaded from the default location. The default location is a file in
       * the director ~/.certs on UNIX machines and certs in the directory where
       * the application is installed in Windows. The format of the files is
       * assumed to be PEM but pkcs7 and pkcs12 may also be supported. 
       *
       * The root certificates are in root.pem.
       * The public key for this user or proxy is in id.pem
       * The private key for this user or proxy is in id_key.pem
       * Any other .pem files are assumed to be other users public certs.
       *
       */

      /* load function return true if they worked, false otherwise */
      bool loadAllCerts( const Data& password,  const Data& dirPath=Data::Empty );
      bool loadRootCerts(  const Data& filePath );

      bool loadMyPublicCert(  const Data& filePath );
      bool loadMyPrivateKey(  const Data& password,  const Data& filePath );
      bool createMyKey( const Data& password, 
                         const Data& filePathPrivateKey=Data::Empty,
                         const Data& filePathPublicKey=Data::Empty );

      bool loadPublicCert(  const Data& filePath=Data::Empty );
      bool savePublicCert( const Data& certName,  const Data& filePath=Data::Empty );
      
      /* stuff for importing certificates from other formats */
      bool importPkcs12Info( const Data& filePathForP12orPFX , const Data& password, 
                             const Data& filePathPublicCert=Data::Empty, 
                             const Data& filePathPrivateKey=Data::Empty  );
      bool importPkcs7Info(  const Data& filePathForP7c, 
                             const Data& filePathCert=Data::Empty);

      /* stuff for exporting certificates to other formats */
      bool exportPkcs12Info( const Data& filePathForP12orPFX , const Data& password, 
                             const Data& filePathPublicCert=Data::Empty, 
                             const Data& filePathPrivateKey=Data::Empty  );
      bool exportMyPkcs7Info(  const Data&  filePathForP7c );
      bool exportPkcs7Info(  const Data& filePathForP7c, 
                             const Data& recipCertName );

      
      /* stuff to build messages 
       *  This is pertty straight forwartd - use ONE of the functions below to
       *  form a new body. */
      Pkcs7Contents* sign( Contents* );
      Pkcs7Contents* encrypt( Contents* , const Data& recipCertName );
      Pkcs7Contents* signAndEncrypt( Contents* , const Data& recipCertName );
      
      /* stuff to receive messages */
      enum SignatureStatus {
         none, // there is no signature 
         isBad,
         trusted, // It is signed with trusted signature 
         caTrusted, // signature is new and is signed by a root we trust 
         notTrusted, // signature is new and is not signed by a CA we
      };

      struct CertificateInfo 
      {
            char email[1024];
            char fingerprint[1024];
            char validFrom[128];
            char validTo[128];
      };
      CertificateInfo getCertificateInfo( Pkcs7Contents* );
      void addCertificate( Pkcs7Contents*  ); // Just puts cert in trust list but does
                                     // not save on disk for future
                                     // sessions. You almost allways don't want
                                     // to use this but should use addAndAve
      void addAndSaveCertificate( Pkcs7Contents*, const Data& filePath=Data::Empty ); // add cert and
                                                                // saves on disk

      Contents* uncode( Pkcs7Contents*,       
                        Data* signedBy, SignatureStatus* sigStat, bool* encryped ); // returns NULL if fails 

   private:
      Contents* uncodeSingle( Pkcs7Contents*, bool verifySig,
                              Data* signedBy, SignatureStatus* sigStat, bool* encryped ); // returns NULL if fails 
               
      Data getPath( const Data& dir, const Data& file );
#ifdef USE_SSL   
      // need a map of certName to certificates

      // root cert list 
      X509_STORE* certAuthorities;

      // my public cert
      X509* publicCert;
      
      // my private key 
      EVP_PKEY* privateKey;
#endif	
};


}

#endif


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 */