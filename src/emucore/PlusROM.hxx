//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef PLUSROM_HXX
#define PLUSROM_HXX

#include "bspf.hxx"
#include "Serializable.hxx"

/**
  Class used to emulate the 'PlusROM' meta-scheme, documented at
    http://pluscart.firmaplus.de/pico/?PlusROM

  This scheme basically wraps a normal bankswitching scheme, but includes
  network functionality.

  Host and path names are stored as 0-terminated strings, located at the
  NMI vector, stored path first and then host next.

  PlusROMs functions use 4 hotspot addresses (before the bankswitching area):
    $1FF0 is for writing a byte to the send buffer (max 256 bytes)
    $1FF1 is for writing a byte to the send buffer and submit the buffer
      to the back end API
    $1FF2 contains the next byte of the response from the host, every read will
      increment the receive buffer pointer (receive buffer is max 256 bytes also!)
    $1FF3 contains the number of (unread) bytes left in the receive buffer
      (these bytes can be from multiple responses)

  @author  Stephen Anthony
*/
class PlusROM : public Serializable
{
  public:
    PlusROM() = default;
    ~PlusROM() override = default;

  public:
    /**
      Determine whether this is actually a PlusROM cart, and if so create
      and initialize all state variables it will use.  This includes
      whether there is a valid hostname and path embedded in the ROM.

      @param image  Pointer to the ROM image
      @param size   The size of the ROM image

      @return  Whether this is actually a valid PlusROM cart
    */
    bool initialize(const ByteBuffer& image, size_t size);

    /**
      Answer whether this is a PlusROM cart.  Note that until the
      initialize method has been called, this will always return false.

      @return  Whether this is actually a PlusROM cart
    */
    bool isValid() const { return myIsPlusROM; }

    /**
      Read from hotspot addresses ($1FF2 and $1FF3).

      @param address  The hotspot where the value should be read
      @param value    The value read from the hotspot

      @return  Indicates whether the peek succeeded or failed
               (ie, whether it hit a hotspot)
               On failure, 'value' is not considered valid
    */
    bool peekHotspot(uInt16 address, uInt8& value);

    /**
      Write to hotspot addresses ($1FF0 and $1FF1).

      @param address  The hotspot where the value should be written
      @param value    The value to be stored at the hotspot

      @return  Indicates whether the poke succeeded or failed
               (ie, whether it hit a hotspot)
    */
    bool pokeHotspot(uInt16 address, uInt8 value);

    /**
      Save the current state of this device to the given Serializer.

      @param out  The Serializer object to use
      @return  False on any errors, else true
    */
    bool save(Serializer& out) const override;

    /**
      Load the current state of this device from the given Serializer.

      @param in  The Serializer object to use
      @return  False on any errors, else true
    */
    bool load(Serializer& in) override;

  private:
    bool myIsPlusROM{false};
    string myPath, myHost;

    std::array<uInt8, 256> myRxBuffer, myTxBuffer;

  private:
    // Following constructors and assignment operators not supported
    PlusROM(const PlusROM&) = delete;
    PlusROM(PlusROM&&) = delete;
    PlusROM& operator=(const PlusROM&) = delete;
    PlusROM& operator=(PlusROM&&) = delete;
};

#endif