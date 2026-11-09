"""Compare reviewed native profile records against actual ELF and full-signature dump.
No device operations, no automatic source changes, no compatibility certification.
"""
import argparse
import copy
import json
import re
import struct
from pathlib import Path
import profile_evidence as evidence

REVIEWED_CODES={'156.0':1792142832,'156.1':1793076318,'157.0':1793360009}

def native_profiles(text):
    array=re.search(r'inline constexpr GameProfile game_profiles\[\]\s*\{(.*?)\n\};',text,re.S)
    if not array:raise ValueError('reviewed profile array missing')
    body=array[1]
    pattern=r'\{"(\d+\.\d+)",(\d+),(\d+),"([a-f0-9]{64})",(\d+),\s*((?:\{0x[0-9a-f]+,\{(?:0x[0-9a-f]+,?)+\}\},?\s*){5})\}'
    records={};at=0
    for match in re.finditer(pattern,body):
        if body[at:match.start()].strip()!=(',' if records else ''):
            raise ValueError('unparsed profile declaration')
        at=match.end()
        version,code,size,sha,layout,hooks_text=match.groups()
        if version in records:raise ValueError('duplicate profile')
        hooks=re.findall(r'\{(0x[0-9a-f]+),\{([^}]+)\}\}',hooks_text)
        if len(hooks)!=len(evidence.HOOKS) or any(len(words.split(','))!=6 for _,words in hooks):
            raise ValueError('incomplete hook prefix')
        records[version]={'version_code':int(code),'size':int(size),'sha':sha,'layout':int(layout),
                          'hooks':{key:(int(rva,16),[int(x,16) for x in words.split(',')])
                                   for key,(rva,words) in zip(evidence.HOOKS,hooks)}}
    if body[at:].strip() not in ('',','):raise ValueError('unparsed profile declaration')
    if set(records)!=set(REVIEWED_CODES):raise ValueError('incomplete/unreviewed profile set')
    if any(records[version]['version_code']!=code for version,code in REVIEWED_CODES.items()):
        raise ValueError('reviewed version/code mismatch')
    return records

def verify(profile,result,elf):
    if profile['layout']!=1 or profile['size']!=result['inputs']['elf']['size'] or profile['sha']!=result['inputs']['elf']['sha256']:
        raise ValueError('binary identity mismatch')
    for key,(rva,words) in profile['hooks'].items():
        expected=result['hooks'][key]['rva']
        if key=='text':
            _,branch=evidence.prefix(elf,expected,4)
            if branch!=struct.pack('<I',0x14000001):raise ValueError('text veneer changed')
            expected+=4
        if rva!=expected:raise ValueError('wrong full-signature entry: '+key)
        _,actual=evidence.prefix(elf,rva)
        if list(struct.unpack('<6I',actual))!=words:raise ValueError('prologue mismatch: '+key)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--game1560',type=Path,required=True);p.add_argument('--game1561',type=Path,required=True)
    p.add_argument('--game1570',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    root=Path(__file__).resolve().parents[1]
    header=root/'native/renderer/game_profiles.h';profiles=native_profiles(header.read_text())
    results={};rejected=0
    for version,folder in [('156.0',a.game1560),('156.1',a.game1561),('157.0',a.game1570)]:
        result=evidence.evidence(folder/'dump/dump.cs',folder/'libil2cpp.so',folder/'global-metadata.dat')
        profile=profiles[version];elf=folder/'libil2cpp.so'
        verify(profile,result,elf);results[version]=result
        mutations=[]
        for key in profile['hooks']:
            bad=copy.deepcopy(profile);rva,words=bad['hooks'][key];bad['hooks'][key]=(rva+4,words);mutations.append(bad)
            bad=copy.deepcopy(profile);bad['hooks'][key][1][0]^=1;mutations.append(bad)
        for key,value in [('size',profile['size']+1),('sha','0'*64),('layout',2)]:
            bad=copy.deepcopy(profile);bad[key]=value;mutations.append(bad)
        for bad in mutations:
            try:verify(bad,result,elf)
            except ValueError:rejected+=1
            else:raise AssertionError('missed source mutation')
    fields=lambda result:{k:v['fields'] for k,v in result['layouts'].items()}
    if any(fields(result)!=fields(results['156.0']) for result in results.values()):
        raise ValueError('shared-layout assumption failed')
    report={'status':'OFFLINE_SOURCE_MATCH_NOT_DEVICE_ACCEPTANCE','header_sha256':evidence.sha(header),
            'shared_layout_classes':len(results['156.0']['layouts']),'rejected_mutations':rejected,'games':results}
    with a.output.open('x',encoding='utf-8') as f:json.dump(report,f,indent=2)
    count=len(results)
    print(f'PASS: {count} exact ELF identities, {len(evidence.HOOKS)*count} full-signature hook bodies/prologues, {count} text veneers, {len(results["156.0"]["layouts"])} identical class layouts, {rejected} rejected mutations; NOT device acceptance')

if __name__=='__main__':main()
